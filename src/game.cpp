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
#include "gamerules.h"
#include "boardaid.h"
#include "button.h"
#include "pegbox.h"
#include "pinbox.h"
#include "solver.h"
#include "message.h"
#include "ctime"

inline void setStateOfList(QList<PegBox *> *boxlist, const BoxState &state_t)
{
	foreach (PegBox *box, *boxlist)
	{
		box->setState(state_t);
	}
}
//-----------------------------------------------------------------------------

inline void setStateOfList(QList<PinBox *> *boxlist, const BoxState &state_t)
{
	foreach (PinBox *box, *boxlist)
	{
		box->setState(state_t);
	}
}
//-----------------------------------------------------------------------------

Game::Game(GameRules *game_rules, BoardAid *board_aid, QWidget *parent):
	QGraphicsView(parent),
	gameState(GameState::None),
	gameRules(game_rules),
	boardAid(board_aid),
	solver(0)
{
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
	if(gameRules->gameMode == GameMode::HVM)
	{
		if (m_filled)
		{
			pinBoxes.first()->setState(BoxState::Current);
			message->setText(tr("Press The Pin Box"));
			if (boardAid->autoCloseRows)
				onPinBoxPressed();
		}
		else
		{
			pinBoxes.first()->setState(BoxState::Future);
			message->setText(tr("Place Your Pegs"));
		}
	}
	else	//gameRules->gameMode == GameMode::MVH
	{
		doneButton->setVisible(m_filled);
		if (m_filled)//the user is not done putting the master code
			message->setText(tr("Press Done"));
		else
			message->setText(tr("Place Your Pegs"));
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

		auto pinbox = new PinBox(gameRules->pegNumber, position);
		scene()->addItem(pinbox);
		pinBoxes.append(pinbox);
		connect(pinbox, SIGNAL(pinBoxPressSignal()), this, SLOT(onPinBoxPressed()));

		position.setX(160-20*gameRules->pegNumber);

		for (int j = 0; j < gameRules->pegNumber; ++j)
		{
			auto pegbox = new PegBox(position+QPoint(j*40, 0));
			scene()->addItem(pegbox);
			codeBoxes.append(pegbox);
		}

		position.setX(276);	//go to right corner for the peg boxes

		auto pegbox = new PegBox(position);
		scene()->addItem(pegbox);
		if (i < gameRules->colorNumber)
		{
			Peg *peg;
			peg = createPeg(pegbox, i);
			peg->setState(PegState::Underneath);
			connect(this, SIGNAL(showIndicatorsSignal()), peg, SLOT(onShowIndicators()));
			createPegForBox(pegbox, i);
			pegbox->setPegState(PegState::Initial);
		}
		else
		{
			createPeg(pegbox, i)->setState(PegState::Plain);
		}
		pegBoxes.append(pegbox);
	}

	for (int i = 0; i < gameRules->pegNumber; ++i)// the last code boxes are for the master code
	{
		auto box = new PegBox(QPoint(160-20*gameRules->pegNumber+i*40, 70));
		scene()->addItem(box);
		masterBoxes.append(box);
	}
}
//-----------------------------------------------------------------------------

void Game::createPegForBox(PegBox *m_box, int m_color)
{
	QPointF pos = m_box->sceneBoundingRect().center();
	if (m_box->hasPeg())
	{
		m_box->setPegColor(m_color);
	} else
	{
		auto peg = new Peg(pos, m_color, &boardAid->indicator);
		scene()->addItem(peg);

		m_box->setPeg(peg);
		connect(peg, SIGNAL(mouseReleaseSignal(Peg *)), this, SLOT(onPegMouseReleased(Peg *)));
		connect(peg, SIGNAL(mouseDoubleClickSignal(Peg*)), this, SLOT(onPegMouseDoubleClicked(Peg*)));
		connect(this, SIGNAL(showIndicatorsSignal()), peg, SLOT(onShowIndicators()));
	}
}
//-----------------------------------------------------------------------------

Peg *Game::createPeg(PegBox *m_box, const int &m_color)
{
	QPointF pos = m_box->sceneBoundingRect().center();
	auto peg = new Peg(pos, m_color, &boardAid->indicator);
	scene()->addItem(peg);
	return peg;
}
//-----------------------------------------------------------------------------

void Game::freezeAllLists()
{
	setStateOfList(&masterBoxes, BoxState::Past);
	setStateOfList(&currentBoxes, BoxState::Past);
	setStateOfList(&pinBoxes, BoxState::Future);
	setStateOfList(&pegBoxes, BoxState::Future);
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

	okButton = new Button(boardAid->boardFont, 36, tr("OK"));
	scene()->addItem(okButton);
	okButton->setZValue(2);
	connect(okButton, SIGNAL(buttonPressed()), this, SLOT(onOkButtonPressed()));
	okButton->setEnabled(false);
	okButton->setVisible(false);

	doneButton = new Button(boardAid->boardFont, 158, tr("Done"));
	doneButton->setPos(79, 118);
	doneButton->setVisible(false);
	doneButton->setZValue(2);
	doneButton->setEnabled(false);
	connect(doneButton, SIGNAL(buttonPressed()), this, SLOT(onDoneButtonPressed()));
	scene()->addItem(doneButton);

	message = new Message(boardAid->boardFont, "#303030");
	scene()->addItem(message);
	message->setPos(20, 0);

	information = new Message(boardAid->boardFont, "#808080", 2);
	scene()->addItem(information);
	information->setPos(20, 506);
	showTranslatedInformation();
	createBoxes();
	scene()->update();
}
//-----------------------------------------------------------------------------

void Game::onPegMouseReleased(Peg *peg)
{
	QPointF position = peg->sceneBoundingRect().center();
	int color = peg->getColor();
	//if same color is not allowed and there is already a color-peg visible, we just ignore drop
	if(!gameRules->sameColorAllowed)
	{
		foreach (PegBox *box, currentBoxes)
		{
			if(!box->sceneBoundingRect().contains(position) &&
					box->isPegVisible() && box->getPegColor() == color)
			{
				boardAid->boardSounds.playPegDropRefuseSound();
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
			boardAid->boardSounds.playPegDropSound();
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
	boardAid->boardSounds.playButtonPressSound();
	gameState = GameState::Running;

	guessElement.guess = "";
	for(int i = 0; i < gameRules->pegNumber; ++i)
		guessElement.guess.append(QString::number(currentBoxes.at(i)->getPegColor()));

	pinBoxes.first()->setPins(guessElement.code, guessElement.guess, gameRules->colorNumber);
	bool isDone = (pinBoxes.first()->getValue() == (gameRules->pegNumber + 1)*(gameRules->pegNumber + 2)/2 - 1);
	pinBoxes.first()->setState(BoxState::Past);
	pinBoxes.removeFirst();	// the pinbox goes to past

	//old row of codes goes to past and are removed from current
	for(int i = 0; i < gameRules->pegNumber; ++i)
	{
		currentBoxes.first()->setState(BoxState::Past);
		currentBoxes.removeFirst();
	}

	if (isDone) //success here
	{
		gameState = GameState::Win;
		message->setText(tr("Success! You Win"));
		freezeAllLists();

	}
	else if (codeBoxes.isEmpty()) //out of more row of codes, a fail
	{
		gameState = GameState::Lose;
		message->setText(tr("Game Over! You Failed"));
		freezeAllLists();
	}
	else // continue the game
	{
		message->setText(tr("Place Your Pegs"));
		for(int i = 0; i < gameRules->pegNumber; ++i) //put the next codeboxes in action
		{
			currentBoxes.append(codeBoxes.first());
			codeBoxes.first()->setState(BoxState::Current);
			codeBoxes.removeFirst();
		}
	}
}
//-----------------------------------------------------------------------------

void Game::changeIndicators()
{
	emit showIndicatorsSignal();
}
//-----------------------------------------------------------------------------

void Game::retranslateTexts()
{
	if(okButton)
	{
		okButton->setLabel(tr("OK"));
		okButton->update();
	}
	if (doneButton)
	{
		doneButton->setLabel(tr("Done"));
		doneButton->update();
	}
	showTranslatedInformation();
}
//-----------------------------------------------------------------------------

void Game::onRevealOnePeg()
{
	if (gameRules->gameMode == GameMode::HVM && gameState == GameState::Running)
	{
		foreach (PegBox *box, masterBoxes)
		{
			if(box->getState() != BoxState::Past)
			{
				if (box == masterBoxes.last())
				{
					gameState = GameState::Running;
					onResigned();
				}
				else
				{
					box->setState(BoxState::Past);
					return;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------

void Game::onResigned()
{
	if (gameRules->gameMode == GameMode::HVM && gameState == GameState::Running)
	{
		gameState = GameState::None;
		message->setText(tr("You Resign"));
		freezeAllLists();
	}
}
//-----------------------------------------------------------------------------

void Game::onOkButtonPressed()
{
	int resp = pinBoxes.first()->getValue();
	if(!solver->setResponse(resp))
	{
		message->setText(tr("Not Possible, Try Again"));
		return;
	}
	boardAid->boardSounds.playButtonPressSound();
	okButton->setVisible(false);

	pinBoxes.first()->setState(BoxState::Past);
	pinBoxes.removeFirst();

	if (solver->done())
	{
		message->setText(tr("Ready To Play"));
		gameState = GameState::None;
		return;
	}

	message->setText(tr("Let Me Think"));
	emit startGuessingSignal();
}
//-----------------------------------------------------------------------------

void Game::onDoneButtonPressed()
{
	boardAid->boardSounds.playButtonPressSound();
	gameState = GameState::Running;

	doneButton->setVisible(false);
	doneButton->setEnabled(false);

	setStateOfList(&currentBoxes, BoxState::Past);
	setStateOfList(&pegBoxes, BoxState::Future);

	guessElement.code = "";
	foreach(PegBox *box, currentBoxes)
		guessElement.code.append(QString::number(box->getPegColor()));

	currentBoxes.clear();
	message->setText(tr("Let Me Think"));
	emit startGuessingSignal();
}
//-----------------------------------------------------------------------------

void Game::onGuessReady()
{
	gameState = GameState::Running;
	showTranslatedInformation();

	if (codeBoxes.isEmpty())
	{
		gameState = GameState::Lose;
		return;
	}
	else
	{
		for(int i = 0; i < gameRules->pegNumber; ++i)
		{
			createPegForBox(codeBoxes.first(), guessElement.guess[i].digitValue());
			codeBoxes.first()->setState(BoxState::Past);
			codeBoxes.removeFirst();
		}
		message->setText(tr("Please Put Pins And Press OK"));
		pinBoxes.first()->setState(BoxState::None);
		okButton->setEnabled(true);
		okButton->setVisible(true);
		okButton->setPos(pinBoxes.first()->pos()-QPoint(0, 39));
	}

	if (boardAid->autoPutPins)
	{
		pinBoxes.first()->setPins(guessElement.code, guessElement.guess, gameRules->colorNumber);
	}
}
//-----------------------------------------------------------------------------

void Game::play()
{
	stop();

	if(gameRules->gameMode == GameMode::MVH)
		playCodeMaster();
	else	//	gameRules->gameMode == GameMode::HVM
		playCodeBreaker();
}
//-----------------------------------------------------------------------------

void Game::stop()
{
	if (solver)
	{
		emit interuptSignal();
		solver->quit();
		solver->wait();
	}
	initializeScene();
	gameState = GameState::None;
}
//-----------------------------------------------------------------------------

void Game::playCodeMaster()
{
	doneButton->setZValue(2);
	doneButton->setVisible(false);
	doneButton->setEnabled(true);

	if (!solver)
	{
		solver = new Solver(gameRules, &guessElement, this);
		connect(solver, SIGNAL(guessDoneSignal()),
				this, SLOT(onGuessReady()));
		connect(this, SIGNAL(startGuessingSignal()), solver, SLOT(onStartGuessing()));
		connect(this, SIGNAL(resetGameSignal()), solver, SLOT(onReset()));
		connect(this, SIGNAL(interuptSignal()), solver, SLOT(onInterupt()));
	}
	emit interuptSignal();
	emit resetGameSignal();

	message->setText(tr("Place Your Pegs"));

	for(int i = 0; i < gameRules->pegNumber; ++i) //initializing currentrow
	{
		currentBoxes.append(masterBoxes.first());
		masterBoxes.first()->setState(BoxState::Current);
		masterBoxes.removeFirst();
	}

	/*	Nothing happening from here till the user fill the master code
	 *	row and press the done button. After the done button is pressed,
	 *	the onDoneButtonPressed is continuing the game
	 */
}
//-----------------------------------------------------------------------------

void Game::playCodeBreaker()
{
	QString digits = "0123456789";
	digits.left(gameRules->colorNumber);
	int remainingNumbers = gameRules->colorNumber;
	qsrand(time(NULL));
	guessElement.code = "";

	foreach (PegBox *box, masterBoxes) //creating a master code to be guessed
	{
		int color = qrand() % remainingNumbers;
		int realcolor = digits.at(color).digitValue();
		guessElement.code.append(QString::number(realcolor));
		createPegForBox(box, realcolor);
		box->setState(BoxState::None);
		if(!gameRules->sameColorAllowed)
		{
			digits.remove(color, 1);
			--remainingNumbers;
		}
	}

	for(int i = 0; i < gameRules->pegNumber; ++i) //initializing currentrow
	{
		currentBoxes.append(codeBoxes.first());
		codeBoxes.first()->setState(BoxState::Current);
		codeBoxes.removeFirst();
	}

	message->setText(tr("Place Your Pegs"));
	// from now on the onPinBoxPushed function continue the game, after the code row is filled
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
	gameRules->algorithm = algorithm_n;
}
//-----------------------------------------------------------------------------

void Game::showTranslatedInformation()
{
	if (gameRules->gameMode == GameMode::MVH)
	{
		if (guessElement.possibles == 1)
		{
			information->setText(tr("The Code Is Cracked!"));
		}
		else if (guessElement.possibles > 10000)
		{
			information->setText(QString("%1    %2: %3").arg(tr("Random Guess")).
								  arg(tr("Remaining")).arg(boardAid->locale.toString(guessElement.possibles)));
		}
		else
		{
			switch (guessElement.algorithm) {
			case Algorithm::MostParts:
				information->setText(QString("%1: %2    %3: %4").arg(tr("Most Parts")).
										  arg(boardAid->locale.toString(guessElement.weight)).arg(tr("Remaining")).
										  arg(boardAid->locale.toString(guessElement.possibles)));
				break;
			case Algorithm::WorstCase:
				information->setText(QString("%1: %2    %3: %4").arg(tr("Worst Case")).
										  arg(boardAid->locale.toString(guessElement.weight)).arg(tr("Remaining")).
										  arg(boardAid->locale.toString(guessElement.possibles)));
				break;
			default:
				information->setText(QString("%1: %2    %3: %4").arg(tr("Expected Size")).
										  arg(boardAid->locale.toString(guessElement.weight)).arg(tr("Remaining")).
										  arg(boardAid->locale.toString(guessElement.possibles)));
				break;
			}

		}
	}
	else
		information->setText(QString("%1: %2   %3: %4   %5: %6").arg(tr("Slots")).
								  arg(boardAid->locale.toString(gameRules->pegNumber)).arg(tr("Colors")).
								  arg(boardAid->locale.toString(gameRules->colorNumber)).arg(tr("Same Color")).
								  arg(gameRules->sameColorAllowed ? tr("Yes"): tr("No")));
}
//-----------------------------------------------------------------------------
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
