/***********************************************************************
 *
 * Copyright (C) 2013 Omid Nikta <omidnikta@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "game.h"
#include "button.h"
#include "pegbox.h"
#include "pinbox.h"
#include "solver.h"
#include "message.h"
#include "ctime"
#include <QMetaType>
#include <QDebug>
#include <QSettings>

#if QT_VERSION >= 0x050000
#include <QtMultimedia/QSoundEffect>
#endif


//-----------------------------------------------------------------------------

inline void setBoxStateOfList(QList<PegBox *> *boxlist, const BoxState &state_t)
{
	foreach (PegBox *box, *boxlist)
	{
		box->setBoxState(state_t);
	}
}
//-----------------------------------------------------------------------------

inline void setBoxStateOfList(QList<PinBox *> *boxlist, const BoxState &state_t)
{
	foreach (PinBox *box, *boxlist)
	{
		box->setBoxState(state_t);
	}
}


Game::Game(QWidget *parent):
	QGraphicsView(parent),
	gameState(GameState::None),
	gameMode(GameMode::Codemaker),
	pegNumber(4),
	colorNumber(6),
	sameColorAllowed(true),
	autoPutPins(false),
	autoCloseRows(false),
	fontSize(QSettings().value("FontSize", 12).toInt()),
	fontName(QSettings().value("FontName", "SansSerif").toString()),
	algorithm(Algorithm::MostParts),
	showColors(QSettings().value("ShowColors", 0).toBool()),
	showIndicators(QSettings().value("ShowIndicators", 0).toBool()),
	indicatorType((IndicatorType) QSettings().value("IndicatorType", 0).toInt()),
	locale(0),
	solver(0)
{
	qRegisterMetaType<Algorithm>("Algorithm");
	auto scene = new QGraphicsScene(this);
	setScene(scene);
	scene->setSceneRect(0, 0, 320, 560);
	fitInView(sceneRect(), Qt::KeepAspectRatio);
	setCacheMode(QGraphicsView::CacheNone);
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setFrameStyle(QFrame::NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setMinimumSize(320, 560);

#if QT_VERSION >= 0x050000
	pegDropSound = new QSoundEffect(this);
	pegDropRefuseSound = new QSoundEffect(this);
	buttonPressSound = new QSoundEffect(this);
	pegDropSound->setSource(QUrl::fromLocalFile("://sounds/pegdrop.wav"));
	pegDropRefuseSound->setSource(QUrl::fromLocalFile("://sounds/pegrefuse.wav"));
	buttonPressSound->setSource(QUrl::fromLocalFile("://sounds/pin.wav"));
#endif
}
//-----------------------------------------------------------------------------

Game::~Game()
{
	if (solver)
	{
		emit interuptSignal();
		solver->quit();
		solver->wait();
		solver->deleteLater();
	}

	scene()->clear();
}
//-----------------------------------------------------------------------------

void Game::codeRowFilled(const bool &m_filled)
{
	switch (gameMode)
	{
	case GameMode::Codebreaker://-----------------------------------------
		switch (m_filled)
		{
		case true:
			pinBoxes.first()->setBoxState(BoxState::Current);
			gameState = GameState::WaittingPinBoxPress;
			message->showMessage("Press The Pin Box ...");
			if (autoCloseRows)
				onPinBoxPressed();
			break;
		default:
			pinBoxes.first()->setBoxState(BoxState::Future);
			message->showMessage(tr("Place Your Pegs ... "));
			break;
		}
		break;
	default://GameMode::Codemaker-------------------------------------
		if (doneButton->isEnabled())//the user is not done putting the master code
		{
			switch (m_filled)
			{
			case true:
				doneButton->setVisible(true);
				message->showMessage(tr("Press Done ..."));
				gameState = GameState::WaittingDoneButtonPress;
				break;
			default:
				doneButton->setVisible(false);
				message->showMessage(tr("Place Your Pegs ... "));
				gameState = GameState::None;
				break;
			}
		}
		break;
	}
}
//-----------------------------------------------------------------------------

void Game::createBoxes()
{
	/*	This function creates boxes and put them in QLists. They are
	 *	stored in QList from bottom to top and left to right.
	 */
	codeBoxes.clear();
	pinBoxes.clear();
	pegBoxes.clear();
	currentBoxes.clear();
	masterBoxes.clear();

	QPoint left_bottom_corner(4, 489);

	for (int i = 0; i < MAX_COLOR_NUMBER; ++i)
	{

		QPoint position = left_bottom_corner - QPoint(0, i*40);

		auto pinbox = new PinBox(pegNumber, position);
		scene()->addItem(pinbox);
		pinBoxes.append(pinbox);
		connect(pinbox, SIGNAL(pinBoxPressSignal()), this, SLOT(onPinBoxPressed()));

		position.setX(160-20*pegNumber);

		for (int j = 0; j < pegNumber; ++j)
		{
			auto pegbox = new PegBox(position+QPoint(j*40, 0));
			scene()->addItem(pegbox);
			codeBoxes.append(pegbox);
		}

		position.setX(276);	//go to right corner for the peg boxes

		auto pegbox = new PegBox(position);
		scene()->addItem(pegbox);
		if (i < colorNumber)
		{
			createPegForBox(pegbox, i, true);//just a background peg
			createPegForBox(pegbox, i);
			pegbox->setPegState(PegState::Initial);
		}
		else
		{
			createPegForBox(pegbox, i, true, true);
		}
		pegBoxes.append(pegbox);
	}

	for (int i = 0; i < pegNumber; ++i)// the last code boxes are for the master code
	{
		auto box = new PegBox(QPoint(160-20*pegNumber+i*40, 70));
		scene()->addItem(box);
		masterBoxes.append(box);
	}
}
//-----------------------------------------------------------------------------

void Game::createPegForBox(PegBox *m_box, int m_color, bool m_underneath, bool m_plain)
{
	QPointF pos = m_box->sceneBoundingRect().center();
	if (m_box->hasPeg())
	{
		m_box->setPegColor(m_color);
	} else
	{
		auto peg = new Peg(pos, m_color, showColors, showIndicators, indicatorType);
		scene()->addItem(peg);

		if(!m_underneath && !m_plain)
		{
			m_box->setPeg(peg);
			connect(peg, SIGNAL(mouseReleaseSignal(Peg *)), this, SLOT(onPegMouseReleased(Peg *)));
			connect(peg, SIGNAL(mouseDoubleClickSignal(Peg*)), this, SLOT(onPegMouseDoubleClicked(Peg*)));
		}

		if (m_underneath)
			peg->setState(PegState::Underneath);
		if (m_plain)
			peg->setState(PegState::Plain);
		else
			connect(this, SIGNAL(showIndicatorsSignal(bool,bool,IndicatorType)), peg, SLOT(onShowIndicators(bool,bool,IndicatorType)));
	}
}
//-----------------------------------------------------------------------------

void Game::initializeScene()
{
	codeBoxes.clear();
	pinBoxes.clear();
	pegBoxes.clear();
	masterBoxes.clear();
	currentBoxes.clear();

	scene()->clear();
	setInteractive(true);

	okButton = new Button(38, tr("OK"), fontName, fontSize);
	scene()->addItem(okButton);
	okButton->setZValue(2);
	connect(okButton, SIGNAL(buttonPressed()), this, SLOT(onOkButtonPressed()));
	okButton->setEnabled(false);
	okButton->setVisible(false);

	doneButton = new Button(160, tr("Done"), fontName, fontSize);
	doneButton->setPos(79, 118);
	doneButton->setVisible(false);
	doneButton->setZValue(2);
	doneButton->setEnabled(false);
	connect(doneButton, SIGNAL(buttonPressed()), this, SLOT(onDoneButtonPressed()));
	scene()->addItem(doneButton);

	message = new Message("#303030", fontName, fontSize);
	scene()->addItem(message);
	message->setPos(20, 0);

	information = new Message("#808080", fontName, fontSize - 2);
	scene()->addItem(information);
	information->setPos(20, 506);
	information->showMessage(QString("%1: %2   %3: %4   %5: %6").arg(tr("Slots")).
							  arg(locale->toString(pegNumber)).arg(tr("Colors")).
							  arg(locale->toString(colorNumber)).arg(tr("Same Color")).
							  arg(sameColorAllowed ? tr("Yes"): tr("No")));
	createBoxes();
	scene()->update();
}
//-----------------------------------------------------------------------------

void Game::onPegMouseReleased(Peg *peg)
{
	QPointF position = peg->sceneBoundingRect().center();
	int color = peg->getColor();
	//if same color is not allowed and there is already a color-peg visible, we just ignore drop
	if(!sameColorAllowed)
	{
		foreach (PegBox *box, currentBoxes)
		{
			if(!box->sceneBoundingRect().contains(position) &&
					box->isPegVisible() && box->getPegColor() == color)
			{
			#if QT_VERSION >= 0x050000
				pegDropRefuseSound->play();
			#endif
				return;
			}
		}
	}

	static bool rowFillState = false;
	bool newRowFillState = true;

	// conversion from float to integer may cause double drop on middle. Flag to do it just once
	bool dropOnlyOnce = true;

	foreach (PegBox *box, currentBoxes)
	{
		if (box->sceneBoundingRect().contains(position) && dropOnlyOnce)
		{
			dropOnlyOnce = false;
		#if QT_VERSION >= 0x050000
			pegDropSound->play();
		#endif
			if(box->hasPeg())
				box->setPegColor(color);
			else
				createPegForBox(box, color);

			box->setPegState(PegState::Visible);
		}//	end if

		if (box->getPegState() != PegState::Visible)
			newRowFillState = false;
	} //	end foreach

	//	if (master) code row state changed, go to codeRowFilled
	if (newRowFillState != rowFillState)
	{
		rowFillState = newRowFillState;
		codeRowFilled(rowFillState);
	}
}
//-----------------------------------------------------------------------------

void Game::onPegMouseDoubleClicked(Peg *peg)
{
	foreach (PegBox *box, currentBoxes)
	{
		if (!box->hasPeg() || box->getPegState() == PegState::Invisible)
		{
			peg->setPos(box->sceneBoundingRect().topLeft());
			break;
		}
	}
}
//-----------------------------------------------------------------------------

void Game::onPinBoxPressed()
{
	/*	This function is the running engine of the Breaker mode.
	 *	If b and c are the number of blacks and whites, then
	 *			w = sum(min(c[i], g[i]) -b
	 *	where c[i] and g[i] are the number of times that the color i
	 *	appears in code/guess, and sum runs over all colors.
	 *	http://mathworld.wolfram.com/Mastermind.html
	 */
#if QT_VERSION >= 0x050000
	buttonPressSound->play();
#endif
	gameState = GameState::Running;

	guess = "";
	for(int i = 0; i < pegNumber; ++i)
		guess.append(QString::number(currentBoxes.at(i)->getPegColor()));

	pinBoxes.first()->setPins(masterCode, guess, colorNumber);
	bool isDone = (pinBoxes.first()->getValue() == (pegNumber + 1)*(pegNumber + 2)/2 - 1);
	pinBoxes.first()->setBoxState(BoxState::Past);
	pinBoxes.removeFirst();	// the pinbox goes to past

	//old row of codes goes to past and are removed from current
	for(int i = 0; i < pegNumber; ++i)
	{
		currentBoxes.first()->setBoxState(BoxState::Past);
		currentBoxes.removeFirst();
	}

	if (isDone) //success here
	{
		gameState = GameState::Win;
		message->showMessage(tr("Success! You Win"));

		setBoxStateOfList(&masterBoxes, BoxState::Past);
		setBoxStateOfList(&currentBoxes, BoxState::Past);
		setBoxStateOfList(&pinBoxes, BoxState::Future);
		setBoxStateOfList(&pegBoxes, BoxState::Future);

	}
	else if (codeBoxes.isEmpty()) //out of more row of codes, a fail
	{
		gameState = GameState::Lose;
		message->showMessage(tr("Game Over! You Failed"));
		setBoxStateOfList(&masterBoxes, BoxState::Past);
		setBoxStateOfList(&currentBoxes, BoxState::Past);
		setBoxStateOfList(&pinBoxes, BoxState::Future);
		setBoxStateOfList(&pegBoxes, BoxState::Future);
	}
	else // continue the game
	{
		message->showMessage(tr("Place Your Pegs ... "));
		for(int i = 0; i < pegNumber; ++i) //put the next codeboxes in action
		{
			currentBoxes.append(codeBoxes.first());
			codeBoxes.first()->setBoxState(BoxState::Current);
			codeBoxes.removeFirst();
			gameState = GameState::WaittingPinBoxPress;
		}
	}
}
//-----------------------------------------------------------------------------

void Game::onShowIndicators(bool show_colors, bool show_indicators, IndicatorType indicator_type)
{
	showColors = show_colors,
	showIndicators = show_indicators;
	indicatorType = indicator_type;
	emit showIndicatorsSignal(showColors, showIndicators, indicatorType);
}
//-----------------------------------------------------------------------------

void Game::onRevealOnePeg()
{
	if (gameMode == GameMode::Codebreaker && (gameState == GameState::Running || gameState == GameState::WaittingPinBoxPress))
	{
		foreach (PegBox *box, masterBoxes)
		{
			if(box->getBoxState() != BoxState::Past)
			{
				if (box == masterBoxes.last())
				{
					gameState = GameState::Running;
					onResigned();
				}
				else
				{
					box->setBoxState(BoxState::Past);
					return;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------

void Game::onResigned()
{
	if (gameMode == GameMode::Codebreaker && (gameState == GameState::Running || gameState == GameState::WaittingPinBoxPress))
	{
		gameState = GameState::None;
		message->showMessage(tr("You Resign"));

		setBoxStateOfList(&masterBoxes, BoxState::Past);
		setBoxStateOfList(&currentBoxes, BoxState::Past);
		setBoxStateOfList(&pinBoxes, BoxState::Future);
		setBoxStateOfList(&pegBoxes, BoxState::Future);
	}
}
//-----------------------------------------------------------------------------

void Game::onOkButtonPressed()
{
	/*	passing user inputed blacks and whites to solver */
	int resp = pinBoxes.first()->getValue();
	if(!solver->setResponse(resp))
	{
		message->showMessage(tr("Not Possible, Try Again..."));
		return;
	}
#if QT_VERSION >= 0x050000
	buttonPressSound->play();
#endif
	okButton->setVisible(false);

	/*	we are done with the pinbox. Make it past and remove it from pinBoxes */
	pinBoxes.first()->setBoxState(BoxState::Past);
	pinBoxes.removeFirst();

	/*	Here we check if the user inputs 4 blacks*/
	if (solver->done())
	{
		message->showMessage(tr("Ready To Play"));
		gameState = GameState::None;
		return;
	}

	message->showMessage(tr("Let Me Think..."));
	gameState = GameState::Guessing;

	emit startGuessingSignal(algorithm);
}
//-----------------------------------------------------------------------------

void Game::onDoneButtonPressed()
{
#if QT_VERSION >= 0x050000
	buttonPressSound->play();
#endif
	gameState = GameState::Running;

	//	we are done with the done button
	doneButton->setVisible(false);
	doneButton->setEnabled(false);

	setBoxStateOfList(&currentBoxes, BoxState::Past);	//	master code boxes become past for no interaction
	setBoxStateOfList(&pegBoxes, BoxState::Future);	//	freezing the peg boxes

	masterCode = "";
	foreach(PegBox *box, currentBoxes)
		masterCode.append(QString::number(box->getPegColor()));

	currentBoxes.clear();
	message->showMessage(tr("Let Me Think..."));

	gameState = GameState::Guessing;
	emit startGuessingSignal(algorithm);
}
//-----------------------------------------------------------------------------

void Game::onGuessReady(const Algorithm &alg, const QString &m_guess,
						 const int &m_possibleSize, const qreal &m_lastWeight)
{
	gameState = GameState::Running;
	guess = m_guess;
	showTranslatedInformation(alg, m_possibleSize, m_lastWeight);

	if (codeBoxes.isEmpty())
	{
		gameState = GameState::Lose; // Hehe, I can solve it always
		message->showMessage(tr("I Lose"));
		return;
	}
	else
	{
		for(int i = 0; i < pegNumber; ++i)
		{
			createPegForBox(codeBoxes.first(), guess[i].digitValue());
			codeBoxes.first()->setBoxState(BoxState::Past);
			codeBoxes.removeFirst();
		}
		message->showMessage(tr("Please Put Pins And Press OK"));
		pinBoxes.first()->setBoxState(BoxState::None);
		okButton->setEnabled(true);
		okButton->setVisible(true);
		okButton->setPos(pinBoxes.first()->pos()-QPoint(0, 39));
	}

	gameState = GameState::WaittingOkButtonPress;

	if (autoPutPins)
	{
		pinBoxes.first()->setPins(masterCode, guess, colorNumber);
	}
}
//-----------------------------------------------------------------------------

void Game::onPreferencesChanged()
{
	fontName =  QSettings().value("FontName", "Sans Serif").toString();
	fontSize = QSettings().value("FontSize", 12).toInt();
#if QT_VERSION >= 0x050000
	qreal volume = (qreal) QSettings().value("Volume", 70).toInt()/100;
	pegDropSound->setVolume(volume);
	pegDropRefuseSound->setVolume(volume);
	buttonPressSound->setVolume(volume);
#endif
}
//-----------------------------------------------------------------------------

void Game::play()
{
	if (solver)
	{
		emit interuptSignal();
		solver->quit();
		solver->wait();
	}
	initializeScene();
	gameState = GameState::None;
	switch (gameMode) {
	case GameMode::Codemaker:
		playCodeMaster();
		break;
	default:
		playCodeBreaker();
		break;
	}
}
//-----------------------------------------------------------------------------

void Game::playCodeMaster()
{
	doneButton->setZValue(2);
	doneButton->setVisible(false);
	doneButton->setEnabled(true);

	if (!solver)
	{
		solver = new Solver(pegNumber, colorNumber, sameColorAllowed, this);
		connect(solver, SIGNAL(guessDoneSignal(Algorithm, QString, int, qreal)),
				this, SLOT(onGuessReady(Algorithm, QString, int, qreal)));
		connect(this, SIGNAL(startGuessingSignal(Algorithm)), solver, SLOT(onStartGuessing(Algorithm)));
		connect(this, SIGNAL(resetGameSignal(int,int,bool)), solver, SLOT(onReset(int,int,bool)));
		connect(this, SIGNAL(interuptSignal()), solver, SLOT(onInterupt()));
	}
	emit interuptSignal();
	emit resetGameSignal(pegNumber, colorNumber, sameColorAllowed);

	message->showMessage(tr("Place Your Pegs ... "));

	for(int i = 0; i < pegNumber; ++i) //initializing currentrow
	{
		currentBoxes.append(masterBoxes.first());
		masterBoxes.first()->setBoxState(BoxState::Current);
		masterBoxes.removeFirst();
	}

	gameState = GameState::WaittingFirstRowFill;
	/*	Nothing happening from here till the user fill the master code
	 *	row and press the done button. After the done button is pressed,
	 *	the onDoneButtonPressed is continuing the game
	 */
}
//-----------------------------------------------------------------------------

void Game::playCodeBreaker()
{
	QString digits = "0123456789";
	digits.left(colorNumber);
	int remainingNumbers = colorNumber;
	qsrand(time(NULL));
	masterCode = "";

	foreach (PegBox *box, masterBoxes) //creating a master code to be guessed
	{
		int color = qrand() % remainingNumbers;
		int realcolor = digits.at(color).digitValue();
		masterCode.append(QString::number(realcolor));
		createPegForBox(box, realcolor);
		box->setBoxState(BoxState::None);
		if(!sameColorAllowed)
		{
			digits.remove(color, 1);
			--remainingNumbers;
		}
	}

	for(int i = 0; i < pegNumber; ++i) //initializing currentrow
	{
		currentBoxes.append(codeBoxes.first());
		codeBoxes.first()->setBoxState(BoxState::Current);
		codeBoxes.removeFirst();
	}

	message->showMessage(tr("Place Your Pegs ... "));
	gameState = GameState::WaittingFirstRowFill;

	// from now on the onPinBoxPushed function continue the game, after the code row is filled
}
//-----------------------------------------------------------------------------

void Game::reset(const int &peg_n, const int &color_n, const GameMode &mode_n, const bool &samecolor,
				  const Algorithm &algorithm_n, const bool &set_pins, const bool &close_row, QLocale *locale_n, const bool &show_colors, const bool &show_indicators,
				  const IndicatorType &indicator_n)
{
	pegNumber = peg_n;
	colorNumber = color_n;
	autoPutPins = set_pins;
	autoCloseRows = close_row;
	gameMode = mode_n;
	showColors = show_colors;
	showIndicators = show_indicators;
	indicatorType = indicator_n;
	algorithm = algorithm_n;
	locale = locale_n;
	sameColorAllowed = samecolor;
}
//-----------------------------------------------------------------------------

void Game::resizeEvent(QResizeEvent *event)
{
	fitInView(sceneRect(), Qt::KeepAspectRatio);
	QGraphicsView::resizeEvent(event);
}

//-----------------------------------------------------------------------------

void Game::setAlgorithm(const Algorithm &algorithm_n)
{
	algorithm = algorithm_n;
}
//-----------------------------------------------------------------------------

void Game::showTranslatedInformation(const Algorithm &m_algorithm, const int &m_possibleSize, const qreal &m_minWeight)
{

	if (m_possibleSize == 1)
	{
		information->showMessage(tr("The Code Is Cracked!"));
	}
	else if (m_possibleSize > 10000)
	{
		information->showMessage(QString("%1    %2: %3").arg(tr("Random Guess")).
							  arg(tr("Remaining")).arg(locale->toString(m_possibleSize)));
	}
	else
	{
		switch (m_algorithm) {
		case Algorithm::MostParts:
			information->showMessage(QString("%1: %2    %3: %4").arg(tr("Most Parts")).
									  arg(locale->toString(m_minWeight)).arg(tr("Remaining")).
									  arg(locale->toString(m_possibleSize)));
			break;
		case Algorithm::WorstCase:
			information->showMessage(QString("%1: %2    %3: %4").arg(tr("Worst Case")).
									  arg(locale->toString(m_minWeight)).arg(tr("Remaining")).
									  arg(locale->toString(m_possibleSize)));
			break;
		default:
			information->showMessage(QString("%1: %2    %3: %4").arg(tr("Expected Size")).
									  arg(locale->toString(m_minWeight)).arg(tr("Remaining")).
									  arg(locale->toString(m_possibleSize)));
			break;
		}

	}

}
//-----------------------------------------------------------------------------

void Game::autoPutPinsCloseRow(const bool &set_pins, const bool &close_row)
{
	autoPutPins = set_pins;
	autoCloseRows = close_row;
}
//-----------------------------------------------------------------------------

void Game::drawBackground(QPainter *painter, const QRectF &rect)
{
	painter->fillRect(rect, QColor(182, 182, 182));// set scene background color

	QLinearGradient topgrad(0, 16, 0, 129);
	topgrad.setColorAt(0.0, QColor(248, 248, 248));
	topgrad.setColorAt(0.6, QColor(184, 184, 184));
	topgrad.setColorAt(1, QColor(212, 212, 212));
	QBrush mTopGrad(topgrad);

	QLinearGradient botgrad(0, 530, 0, 557);
	botgrad.setColorAt(0.0, QColor(204, 204, 204));
	botgrad.setColorAt(0.3, QColor(206, 206, 206));
	botgrad.setColorAt(1.0, QColor(180, 180, 180));
	QBrush mBotGrad(botgrad);

	QLinearGradient lgrad(0, 190, 320, 370);
	lgrad.setColorAt(0.0, QColor(240, 240, 240));
	lgrad.setColorAt(0.49, QColor(240, 240, 240));
	lgrad.setColorAt(0.50, QColor(80, 80, 80));
	lgrad.setColorAt(1.0, QColor(80, 80, 80));
	QPen mFramePen = QPen(QBrush(lgrad), 1);

	painter->setPen(Qt::NoPen);

	QRectF cr(3, 3, 314, 554);
	QPainterPath cpath;
	cpath.addRoundedRect(cr, 10.1, 10.1);
	painter->setClipPath(cpath);
	painter->setClipping(true);

	painter->setBrush(mTopGrad);
	painter->drawRect(QRect(4, 4, 312, 125));
	painter->setBrush(QBrush(QColor(112, 112, 112)));
	painter->drawRect(QRect(4, 128, 318, 1));

	painter->setPen(Qt::NoPen);

	QLinearGradient rowgrad(4, 129, 4, 169);
	rowgrad.setColorAt(0.0, QColor(154, 154, 154));
	rowgrad.setColorAt(1.0, QColor(143, 143, 143));
	rowgrad.setSpread(QGradient::RepeatSpread);
	painter->setBrush(QBrush(rowgrad));
	painter->drawRect(QRectF(4, 129, 318, 400));

	painter->setPen(QColor(135, 135, 135));
	for(int i = 0; i < 11; ++i)
	{
		painter->drawLine(5, 128+i*40, 321, 128+i*40);
	}

	painter->setBrush(mBotGrad);
	painter->drawRect(QRect(1, 528, 318, 28));
	painter->setBrush(QBrush(QColor(239, 239, 239)));
	painter->setClipping(false);

	QRectF rightShadow(3.5, 3.5, 313, 553);
	painter->setBrush(Qt::NoBrush);
	painter->setPen(mFramePen);
	painter->drawRoundedRect(rightShadow, 9.8, 9.8);

	QLinearGradient solgrad(50, 70, 50, 108);
	solgrad.setColorAt(0.0, QColor(154, 154, 154));
	solgrad.setColorAt(1.0, QColor(148, 148, 148));
	QBrush solBgBrush(solgrad);

	QLinearGradient framegrad = QLinearGradient(50, 70, 50, 110);
	framegrad.setColorAt(0.0, QColor(78, 78, 78));
	framegrad.setColorAt(1.0, QColor(235, 235, 235));
	QPen solFramePen(QBrush(framegrad), 1);

	painter->setBrush(solBgBrush);
	painter->setPen(solFramePen);

	QRectF codeRowContainer(41, 68, 235, 42);
	QRectF mRectC(42, 69, 235, 41.5);

	painter->drawRoundedRect(codeRowContainer, 21,21);
	painter->setBrush(solgrad);
	painter->drawRoundedRect(mRectC, 20, 20);
	painter->setRenderHint(QPainter::TextAntialiasing, true);
}
