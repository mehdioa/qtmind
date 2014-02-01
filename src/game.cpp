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
		box->setState(state_t);
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
			pinBoxes.at(playedMoves)->setState(BoxState::Current);
			gameState = GameState::WaittingPinboxPress;
			showMessage();

			if (boardAid->autoCloseRows)
				onPinBoxPressed();
		}
		else
		{
			pinBoxes.at(playedMoves)->setState(BoxState::Future);

			if (playedMoves == 0)
				gameState = GameState::WaittingFirstRowFill;
			else
				gameState = GameState::WaittingCodeRowFill;

			showMessage();
		}
	}
	else	//gameRules->gameMode == GameMode::MVH
	{
		doneButton->setVisible(m_filled);

		if (m_filled)//the user is not done putting the master code
			gameState = GameState::WaittingDoneButtonPress;
		else
			gameState = GameState::WaittingHiddenCodeFill;

		showMessage();
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
			codeBoxes.append(createPegBox(position+QPoint(j*40, 0)));
		}

		position.setX(277);	//go to right corner for the peg boxes

		PegBox *pegbox = createPegBox(position);
		pegBoxes.append(pegbox);
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
	}

	for (int i = 0; i < gameRules->pegNumber; ++i)// the last code boxes are for the master code
	{
		masterBoxes.append(createPegBox(QPoint(160-20*gameRules->pegNumber+i*40, 70)));
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
		Peg *peg = createPeg(pos, m_color);
		m_box->setPeg(peg);
		connect(peg, SIGNAL(mouseReleaseSignal(Peg *)), this, SLOT(onPegMouseReleased(Peg *)));
		connect(peg, SIGNAL(mouseDoubleClickSignal(Peg*)), this, SLOT(onPegMouseDoubleClicked(Peg*)));
		connect(this, SIGNAL(showIndicatorsSignal()), peg, SLOT(onShowIndicators()));
	}
}
//-----------------------------------------------------------------------------

PegBox *Game::createPegBox(const QPoint &m_position)
{
	auto pegbox = new PegBox(m_position);
	scene()->addItem(pegbox);
	return pegbox;
}
//-----------------------------------------------------------------------------

Peg *Game::createPeg(const QPointF &m_position, const int &m_color)
{
	auto peg = new Peg(m_position, m_color, &boardAid->indicator);
	scene()->addItem(peg);
	return peg;
}
//-----------------------------------------------------------------------------

Peg *Game::createPeg(PegBox *m_box, const int &m_color)
{
	return createPeg(m_box->sceneBoundingRect().center(), m_color);
}
//-----------------------------------------------------------------------------

void Game::freezeScene()
{
	setStateOfList(&masterBoxes, BoxState::Past);
	setStateOfList(&currentBoxes, BoxState::Past);
	pinBoxes.at(playedMoves)->setState(BoxState::Past);
	setStateOfList(&pegBoxes, BoxState::Future);
}
//-----------------------------------------------------------------------------

void Game::setNextRowInAction()
{
	currentBoxes.clear();
	for(int i = playedMoves*gameRules->pegNumber; i < (playedMoves+1)*gameRules->pegNumber; ++i)
		currentBoxes.append(codeBoxes.at(i));
	setStateOfList(&currentBoxes, BoxState::Current);
	gameState = GameState::WaittingCodeRowFill;
}
//-----------------------------------------------------------------------------

void Game::getNextGuess()
{
	gameState = GameState::Thinking;
	emit startGuessingSignal();
}
//-----------------------------------------------------------------------------

Player Game::winner() const
{
	if (pinBoxes.at(playedMoves)->getValue() == (gameRules->pegNumber + 1)*(gameRules->pegNumber + 2)/2 - 1)
		return Player::CodeBreaker;
	else if (playedMoves >= MAX_COLOR_NUMBER - 1)
		return Player::CodeMaker;
	else
		return Player::None;
}
//-----------------------------------------------------------------------------

void Game::initializeScene()
{
	codeBoxes.clear();
	pinBoxes.clear();
	pegBoxes.clear();
	masterBoxes.clear();
	currentBoxes.clear();
	playedMoves = 0;

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

	information = new Message(boardAid->boardFont, "#808080", 4);
	scene()->addItem(information);
	information->setPos(20, 506);
	showInformation();
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
		foreach(PegBox *box, currentBoxes)
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

	foreach(PegBox *box, currentBoxes)
	{
		if (box->sceneBoundingRect().contains(position) && dropOnlyOnce)
		{
			dropOnlyOnce = false;
			if(!box->hasPeg())
			{
				createPegForBox(box, color);
				boardAid->boardSounds.playPegDropSound();
			}
			else if(box->getPegColor() != color)
			{
				box->setPegColor(color);
				boardAid->boardSounds.playPegDropSound();
			}

			box->setPegState(PegState::Visible);
		}//	end if

		if (box->getPegState() != PegState::Visible)
			newRowFillState = false;
	} //	end for

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
	foreach(PegBox *box, currentBoxes)
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

	pinBoxes.at(playedMoves)->setPins(guessElement.code, guessElement.guess, gameRules->colorNumber);
	pinBoxes.at(playedMoves)->setState(BoxState::Past);

	setStateOfList(&currentBoxes, BoxState::Past);
	currentBoxes.clear();

	switch (winner()) {
	case Player::CodeBreaker:
		gameState = GameState::Win;
		freezeScene();
		break;
	case Player::CodeMaker:
		gameState = GameState::Lose;
		freezeScene();
		break;
	default:
		++playedMoves;
		setNextRowInAction();
		break;
	}

	showMessage();
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
	showInformation();
	showMessage();
}
//-----------------------------------------------------------------------------

bool Game::isRunning()
{
	switch (gameState) {
	case GameState::None:
	case GameState::Win:
	case GameState::Lose:
	case GameState::Resign:
	case GameState::WaittingFirstRowFill:
	case GameState::WaittingHiddenCodeFill:
	case GameState::WaittingDoneButtonPress:
		return false;
	default:
		return true;
	}
}
//-----------------------------------------------------------------------------

void Game::onRevealOnePeg()
{
	if (gameRules->gameMode == GameMode::HVM && isRunning())
	{
		foreach(PegBox *box, masterBoxes)
		{
			if(box->getState() != BoxState::Past)
			{
				if (box == masterBoxes.last())
				{
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
	if (gameRules->gameMode == GameMode::HVM && isRunning())
	{
		gameState = GameState::Resign;
		showMessage();
		freezeScene();
	}
}
//-----------------------------------------------------------------------------

void Game::onOkButtonPressed()
{
	int resp = pinBoxes.at(playedMoves)->getValue();
	if(!solver->setResponse(resp))
	{
		message->setText(tr("Not Possible, Try Again"));
		return;
	}
	boardAid->boardSounds.playButtonPressSound();
	okButton->setVisible(false);

	pinBoxes.at(playedMoves)->setState(BoxState::Past);

	switch (winner()) {
	case Player::CodeBreaker:
		gameState = GameState::Win;
		freezeScene();
		break;
	case Player::CodeMaker:
		gameState = GameState::Lose;
		freezeScene();
		break;
	default:
		++playedMoves;
		getNextGuess();
		break;
	}

	showMessage();
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
	getNextGuess();
	showMessage();
}
//-----------------------------------------------------------------------------

void Game::onGuessReady()
{
	gameState = GameState::Running;
	showInformation();

	int box_index = playedMoves*gameRules->pegNumber;
	for(int i = 0; i < gameRules->pegNumber; ++i)
	{
		createPegForBox(codeBoxes.at(box_index + i), guessElement.guess[i].digitValue());
		codeBoxes.at(box_index + i)->setState(BoxState::Past);
	}
	gameState = GameState::WaittingOkButtonPress;
	showMessage();
	pinBoxes.at(playedMoves)->setState(BoxState::None);
	okButton->setEnabled(true);
	okButton->setVisible(true);
	okButton->setPos(pinBoxes.at(playedMoves)->pos()-QPoint(0, 39));

	if (boardAid->autoPutPins)
		pinBoxes.at(playedMoves)->setPins(guessElement.code, guessElement.guess, gameRules->colorNumber);
}
//-----------------------------------------------------------------------------

void Game::play()
{
	stop();

	if(gameRules->gameMode == GameMode::MVH)
		playMVH();
	else // (gameRules->gameMode == GameMode::HVM)
		playHVM();
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

void Game::playMVH()
{
	doneButton->setZValue(2);
	doneButton->setVisible(false);
	doneButton->setEnabled(true);

	if (!solver)
	{
		solver = new Solver(gameRules, &guessElement, this);
		connect(solver, SIGNAL(guessDoneSignal()), this, SLOT(onGuessReady()));
		connect(this, SIGNAL(startGuessingSignal()), solver, SLOT(onStartGuessing()));
		connect(this, SIGNAL(resetGameSignal()), solver, SLOT(onReset()));
		connect(this, SIGNAL(interuptSignal()), solver, SLOT(onInterupt()));
	}
	emit interuptSignal();
	emit resetGameSignal();

	gameState = GameState::WaittingHiddenCodeFill;
	showMessage();
	for(int i = 0; i < gameRules->pegNumber; ++i) //initializing currentrow
	{
		currentBoxes.append(masterBoxes.at(i));
		masterBoxes.at(i)->setState(BoxState::Current);
	}

	/*	Nothing happening from here till the user fill the master code
	 *	row and press the done button. After the done button is pressed,
	 *	the onDoneButtonPressed is continuing the game
	 */
}
//-----------------------------------------------------------------------------

void Game::playHVM()
{
	QString digits = "0123456789";
	digits.left(gameRules->colorNumber);
	int remainingNumbers = gameRules->colorNumber;
	qsrand(time(NULL));
	guessElement.code = "";

	foreach(PegBox *box, masterBoxes) //creating a master code to be guessed
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
	setNextRowInAction();
	showMessage();
	gameState = GameState::WaittingFirstRowFill;
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

void Game::showInformation()
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
		information->setText(QString("%1: %2   %3: %4   %5: %6").arg(tr("Slots", "", gameRules->pegNumber)).
								  arg(boardAid->locale.toString(gameRules->pegNumber)).arg(tr("Colors", "", gameRules->colorNumber)).
								  arg(boardAid->locale.toString(gameRules->colorNumber)).arg(tr("Same Colors")).
							 arg(gameRules->sameColorAllowed ? tr("Yes"): tr("No")));
}
//-----------------------------------------------------------------------------

void Game::showMessage()
{
	bool is_MVH = (gameRules->gameMode == GameMode::MVH);
	switch (gameState) {
	case GameState::Win:
		if (is_MVH)
			message->setText(tr("Success! I Win"));
		else
			message->setText(tr("Success! You Win"));
		break;
	case GameState::Lose:
		if (is_MVH)
			message->setText(tr("Game Over! I Failed"));
		else
			message->setText(tr("Game Over! You Failed"));
		break;
	case GameState::Resign:
		message->setText(tr("You Resign"));
		break;
	case GameState::Thinking:
		message->setText(tr("Let Me Think"));
		break;
	case GameState::WaittingPinboxPress:
		message->setText(tr("Press The Pin Box"));
		break;
	case GameState::WaittingOkButtonPress:
		message->setText(tr("Please Put Your Pins And Press OK"));
		break;
	case GameState::WaittingDoneButtonPress:
		message->setText(tr("Press Done"));
		break;
	default:
		message->setText(tr("Place Your Pegs"));
		break;
	}
}
//-----------------------------------------------------------------------------

void Game::drawBackground(QPainter *painter, const QRectF &rect)
{
	painter->fillRect(rect, QColor(182, 182, 182));// set scene background color
	painter->setPen(Qt::NoPen);

	QRectF cr(3, 3, 314, 554);
	QPainterPath cpath;
	cpath.addRoundedRect(cr, 10.1, 10.1);
	painter->setClipPath(cpath);
	painter->setClipping(true);

	QLinearGradient top_grad(0, 16, 0, 129);
	top_grad.setColorAt(0.0, QColor(248, 248, 248));
	top_grad.setColorAt(0.6, QColor(184, 184, 184));
	top_grad.setColorAt(1, QColor(212, 212, 212));
	painter->setBrush(QBrush(top_grad));
	painter->drawRect(QRect(4, 4, 312, 125));
	painter->setBrush(QBrush(QColor(112, 112, 112)));
	painter->drawRect(QRect(4, 128, 318, 1));

	painter->setPen(Qt::NoPen);
	painter->setBrush(QBrush(QColor(154, 154, 154)));
	painter->drawRect(QRectF(4, 129, 318, 400));

	painter->setPen(QColor(135, 135, 135));
	for(int i = 0; i < 11; ++i)
		painter->drawLine(5, 128+i*40, 321, 128+i*40);

	QLinearGradient bot_grad(0, 530, 0, 557);
	bot_grad.setColorAt(0.0, QColor(204, 204, 204));
	bot_grad.setColorAt(0.3, QColor(206, 206, 206));
	bot_grad.setColorAt(1.0, QColor(180, 180, 180));
	painter->setBrush(QBrush(bot_grad));
	painter->drawRect(QRect(1, 528, 318, 28));
	painter->setBrush(QBrush(QColor(239, 239, 239)));
	painter->setClipping(false);

	QLinearGradient frame_grad(0, 190, 320, 370);
	frame_grad.setColorAt(0.0, QColor(240, 240, 240));
	frame_grad.setColorAt(0.49, QColor(240, 240, 240));
	frame_grad.setColorAt(0.50, QColor(80, 80, 80));
	frame_grad.setColorAt(1.0, QColor(80, 80, 80));
	QPen frame_pen = QPen(QBrush(frame_grad), 1);
	QRectF right_shadow(3.5, 3.5, 313, 553);
	painter->setBrush(Qt::NoBrush);
	painter->setPen(frame_pen);
	painter->drawRoundedRect(right_shadow, 9.8, 9.8);

	QLinearGradient sol_grad(50, 70, 50, 108);
	sol_grad.setColorAt(0.0, QColor(154, 154, 154));
	sol_grad.setColorAt(1.0, QColor(148, 148, 148));
	QBrush solBgBrush(sol_grad);

	QLinearGradient sol_frame_grad = QLinearGradient(50, 70, 50, 110);
	sol_frame_grad.setColorAt(0.0, QColor(78, 78, 78));
	sol_frame_grad.setColorAt(1.0, QColor(235, 235, 235));
	QPen sol_frame_pen(QBrush(sol_frame_grad), 1);

	painter->setBrush(solBgBrush);
	painter->setPen(sol_frame_pen);

	QRectF sol_container(41, 68, 235, 42);
	QRectF sol_frame(42, 69, 235, 41.5);

	painter->drawRoundedRect(sol_container, 21,21);
	painter->setBrush(sol_grad);
	painter->drawRoundedRect(sol_frame, 20, 20);
	painter->setRenderHint(QPainter::TextAntialiasing, true);
}
