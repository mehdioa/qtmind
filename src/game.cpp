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
#include "rules.h"
#include "board.h"
#include "button.h"
#include "pegbox.h"
#include "pinbox.h"
#include "solver.h"
#include "message.h"
#include "ctime"

inline static void setStateOfList(QList<PegBox *> *boxlist, const Box::State &state_t)
{
	foreach (PegBox *box, *boxlist)
		box->setState(state_t);
}

Game::Game():
	QGraphicsView(),
	state(State::None),
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
}

Game::~Game()
{
	if (solver) {
		emit interuptSignal();
		solver->quit();
		solver->wait();
		solver->deleteLater();
	}

	scene()->clear();
}

void Game::codeRowFilled(const bool &m_filled)
{
	if(Rules::instance()->getMode() == Mode::HVM) {
		okButton->setVisible(m_filled);
		okButton->setEnabled(m_filled);
		if (m_filled) {
			state = State::WaittingPinboxPress;
			showMessage();

			if (Board::instance()->isAutoCloseRows())
				onOkButtonPressed();
		} else {
			if (playedMoves == 0)
				state = State::WaittingFirstRowFill;
			else
				state = State::WaittingCodeRowFill;

			showMessage();
		}
	} else {	//Rules::instance()->getMode() == Mode::MVH
		doneButton->setVisible(m_filled);

		if (m_filled)//the user is not done putting the master code
			state = State::WaittingDoneButtonPress;
		else
			state = State::WaittingHiddenCodeFill;

		showMessage();
	}
}

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

	for (int i = 0; i < MAX_COLOR_NUMBER; ++i) {
		QPoint position = left_bottom_corner - QPoint(0, i*40);

		auto pinbox = new PinBox(Rules::instance()->getPegs(), position);
		scene()->addItem(pinbox);
		pinBoxes.append(pinbox);

		position.setX(160-20*Rules::instance()->getPegs());

		for (int j = 0; j < Rules::instance()->getPegs(); ++j) {
			codeBoxes.append(createPegBox(position+QPoint(j*40, 0)));
		}

		position.setX(277);	//go to right corner for the peg boxes

		PegBox *pegbox = createPegBox(position);
		pegBoxes.append(pegbox);
		if (i < Rules::instance()->getColors()) {
			Peg *peg;
			peg = createPeg(pegbox, i);
			peg->setState(Peg::State::Underneath);
			connect(this, SIGNAL(showIndicatorsSignal()), peg, SLOT(onShowIndicators()));
			createPegForBox(pegbox, i);
			pegbox->setPegState(Peg::State::Initial);
		} else {
			createPeg(pegbox, i)->setState(Peg::State::Plain);
		}
	}

	// the last code boxes are for the master code
	for (int i = 0; i < Rules::instance()->getPegs(); ++i) {
		masterBoxes.append(createPegBox(QPoint(160-20*Rules::instance()->getPegs()+i*40, 70)));
	}
}

void Game::createPegForBox(PegBox *m_box, int m_color)
{
	QPointF pos = m_box->sceneBoundingRect().center();
	if (m_box->hasPeg()) {
		m_box->setPegColor(m_color);
	} else {
		Peg *peg = createPeg(pos, m_color);
		m_box->setPeg(peg);
		connect(peg, SIGNAL(mouseReleaseSignal(Peg *)), this, SLOT(onPegMouseReleased(Peg *)));
		connect(peg, SIGNAL(mouseDoubleClickSignal(Peg*)), this, SLOT(onPegMouseDoubleClicked(Peg*)));
		connect(this, SIGNAL(showIndicatorsSignal()), peg, SLOT(onShowIndicators()));
	}
}

PegBox *Game::createPegBox(const QPoint &m_position)
{
	auto pegbox = new PegBox(m_position);
	scene()->addItem(pegbox);
	return pegbox;
}

Peg *Game::createPeg(const QPointF &m_position, const int &m_color)
{
	auto peg = new Peg(m_position, m_color);
	scene()->addItem(peg);
	return peg;
}

Peg *Game::createPeg(PegBox *m_box, const int &m_color)
{
	return createPeg(m_box->sceneBoundingRect().center(), m_color);
}

void Game::freezeScene()
{
	setStateOfList(&masterBoxes, Box::State::Past);
	setStateOfList(&currentBoxes, Box::State::Past);
	pinBoxes.at(playedMoves)->setState(Box::State::Past);
	setStateOfList(&pegBoxes, Box::State::Future);
}

void Game::setNextRowInAction()
{
	currentBoxes.clear();
	for(int i = playedMoves*Rules::instance()->getPegs(); i < (playedMoves+1)*Rules::instance()->getPegs(); ++i)
		currentBoxes.append(codeBoxes.at(i));
	setStateOfList(&currentBoxes, Box::State::Current);
	state = State::WaittingCodeRowFill;
}

void Game::getNextGuess()
{
	state = State::Thinking;
	emit startGuessingSignal();
}

Game::Player Game::winner() const
{
	int blacks, whites;
	pinBoxes.at(playedMoves)->getValue(blacks, whites);
	if (blacks == Rules::instance()->getPegs())
		return Player::CodeBreaker;
	else if (playedMoves >= MAX_COLOR_NUMBER - 1)
		return Player::CodeMaker;
	else
		return Player::None;
}

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

	okButton = new Button(36, tr("OK"));
	scene()->addItem(okButton);
	okButton->setZValue(2);
	connect(okButton, SIGNAL(buttonPressed()), this, SLOT(onOkButtonPressed()));
	okButton->setEnabled(false);
	okButton->setVisible(false);

	doneButton = new Button(158, tr("Done"));
	doneButton->setPos(79, 118);
	doneButton->setVisible(false);
	doneButton->setZValue(2);
	doneButton->setEnabled(false);
	connect(doneButton, SIGNAL(buttonPressed()), this, SLOT(onDoneButtonPressed()));
	scene()->addItem(doneButton);

	message = new Message("#303030");
	scene()->addItem(message);
	message->setPos(20, 0);

	information = new Message("#808080", 4);
	scene()->addItem(information);
	information->setPos(20, 506);
	showInformation();
	createBoxes();
	scene()->update();
}

void Game::onPegMouseReleased(Peg *peg)
{
	QPointF position = peg->sceneBoundingRect().center();
	int color = peg->getColor();
	//if same color is not allowed and there is already a color-peg visible, we just ignore drop
	if(!Rules::instance()->isSameColor()) {
		foreach(PegBox *box, currentBoxes) {
			if(!box->sceneBoundingRect().contains(position) &&
					box->isPegVisible() && box->getPegColor() == color) {
				Board::instance()->play(Board::Sound::PegDropRefuse);
				return;
			}
		}
	}

	static bool rowFillState = false;
	bool newRowFillState = true;

	// conversion from float to integer may cause double drop on middle. Flag to do it just once
	bool dropOnlyOnce = true;

	foreach(PegBox *box, currentBoxes) {
		if (box->sceneBoundingRect().contains(position) && dropOnlyOnce) {
			dropOnlyOnce = false;
			createPegForBox(box, color);
			Board::instance()->play(Board::Sound::PegDrop);

			box->setPegState(Peg::State::Visible);
		}//	end if

		if (box->getPegState() != Peg::State::Visible)
			newRowFillState = false;
	} //	end for

	//	if (master) code row state changed, go to codeRowFilled
	if (newRowFillState != rowFillState) {
		rowFillState = newRowFillState;
		codeRowFilled(rowFillState);
	}
}

void Game::onPegMouseDoubleClicked(Peg *peg)
{
	foreach(PegBox *box, currentBoxes) {
		if (!box->hasPeg() || box->getPegState() == Peg::State::Invisible) {
			peg->setPos(box->sceneBoundingRect().topLeft());
			break;
		}
	}
}

void Game::changeIndicators()
{
	emit showIndicatorsSignal();
}

void Game::retranslateTexts()
{
	if(okButton) {
		okButton->setLabel(tr("OK"));
		okButton->update();
	}
	if (doneButton) {
		doneButton->setLabel(tr("Done"));
		doneButton->update();
	}
	showInformation();
	showMessage();
}

bool Game::isRunning()
{
	switch (state) {
	case State::None:
	case State::Win:
	case State::Lose:
	case State::Resign:
	case State::WaittingFirstRowFill:
	case State::WaittingHiddenCodeFill:
	case State::WaittingDoneButtonPress:
		return false;
	default:
		return true;
	}
}

void Game::onRevealOnePeg()
{
	if (Rules::instance()->getMode() == Mode::HVM && isRunning()) {
		foreach(PegBox *box, masterBoxes) {
			if(box->getState() != Box::State::Past) {
				if (box == masterBoxes.last()) {
					onResigned();
				} else {
					box->setState(Box::State::Past);
					return;
				}
			}
		}
	}
}

void Game::onResigned()
{
	if (Rules::instance()->getMode() == Mode::HVM && isRunning()) {
		state = State::Resign;
		showMessage();
		freezeScene();
	}
}

void Game::onOkButtonPressed()
{
	if(Rules::instance()->getMode() == Mode::MVH) {
		int blacks, whites;
		pinBoxes.at(playedMoves)->getValue(blacks, whites);
		if(!solver->setResponse(blacks, whites)) {
			message->setText(tr("Not Possible, Try Again"));
			return;
		}
		Board::instance()->play(Board::Sound::ButtonPress);
		okButton->setVisible(false);

		pinBoxes.at(playedMoves)->setState(Box::State::Past);

		switch (winner()) {
		case Player::CodeBreaker:
			state = State::Win;
			freezeScene();
			break;
		case Player::CodeMaker:
			state = State::Lose;
			freezeScene();
			break;
		default:
			++playedMoves;
			getNextGuess();
			break;
		}

		showMessage();
	} else {
		Board::instance()->play(Board::Sound::ButtonPress);
		state = State::Running;

		unsigned char new_guess[MAX_SLOT_NUMBER];
		for(int i = 0; i < Rules::instance()->getPegs(); ++i)
			new_guess[i] = currentBoxes.at(i)->getPegColor();
		Guess::instance()->setGuess(new_guess);

		pinBoxes.at(playedMoves)->setPins();
		pinBoxes.at(playedMoves)->setState(Box::State::Past);

		setStateOfList(&currentBoxes, Box::State::Past);
		currentBoxes.clear();

		switch (winner()) {
		case Player::CodeBreaker:
			state = State::Win;
			freezeScene();
			break;
		case Player::CodeMaker:
			state = State::Lose;
			freezeScene();
			break;
		default:
			++playedMoves;
			setNextRowInAction();
			break;
		}

		showMessage();
		okButton->setVisible(false);
		okButton->setPos(pinBoxes.at(playedMoves)->pos() + QPoint(0, 1));
	}
}

void Game::onDoneButtonPressed()
{
	Board::instance()->play(Board::Sound::ButtonPress);
	state = State::Running;

	doneButton->setVisible(false);
	doneButton->setEnabled(false);

	setStateOfList(&currentBoxes, Box::State::Past);
	setStateOfList(&pegBoxes, Box::State::Future);

	unsigned char new_guess[MAX_SLOT_NUMBER];
	for(int i = 0; i < Rules::instance()->getPegs(); ++i)
		new_guess[i] = currentBoxes.at(i)->getPegColor();
	Guess::instance()->setCode(new_guess);

	currentBoxes.clear();
	getNextGuess();
	showMessage();
}

void Game::onGuessReady()
{
	state = State::Running;
	showInformation();

	int box_index = playedMoves*Rules::instance()->getPegs();
	for(int i = 0; i < Rules::instance()->getPegs(); ++i) {
		createPegForBox(codeBoxes.at(box_index + i), Guess::instance()->guess[i]);
		codeBoxes.at(box_index + i)->setState(Box::State::Past);
	}
	state = State::WaittingOkButtonPress;
	showMessage();
	pinBoxes.at(playedMoves)->setState(Box::State::None);
	okButton->setEnabled(true);
	okButton->setVisible(true);
	okButton->setPos(pinBoxes.at(playedMoves)->pos()-QPoint(0, 39));

	if (Board::instance()->isAutoPutPins())
		pinBoxes.at(playedMoves)->setPins();
}

void Game::play()
{
	stop();

	if(Rules::instance()->getMode() == Mode::MVH)
		playMVH();
	else // (Rules::instance()->getMode() == Mode::HVM)
		playHVM();
}

void Game::stop()
{
	if (solver) {
		emit interuptSignal();
		solver->quit();
		solver->wait();
	}
	initializeScene();
	state = State::None;
}

void Game::playMVH()
{
	doneButton->setZValue(2);
	doneButton->setVisible(false);
	doneButton->setEnabled(true);

	if (!solver) {
		solver = new Solver(this);
		connect(solver, SIGNAL(guessDoneSignal()), this, SLOT(onGuessReady()));
		connect(this, SIGNAL(startGuessingSignal()), solver, SLOT(onStartGuessing()));
		connect(this, SIGNAL(resetGameSignal()), solver, SLOT(onReset()));
		connect(this, SIGNAL(interuptSignal()), solver, SLOT(onInterupt()));
	}
	emit interuptSignal();
	emit resetGameSignal();

	state = State::WaittingHiddenCodeFill;
	showMessage();
	showInformation();

	//initializing currentrow
	for(int i = 0; i < Rules::instance()->getPegs(); ++i) {
		currentBoxes.append(masterBoxes.at(i));
		masterBoxes.at(i)->setState(Box::State::Current);
	}

	/*	Nothing happening from here till the user fill the master code
	 *	row and press the done button. After the done button is pressed,
	 *	the onDoneButtonPressed is continuing the game
	 */
}

void Game::playHVM()
{
	qsrand(time(NULL));
	int remaining_colors = Rules::instance()->getColors();
	unsigned char hidden_code[MAX_SLOT_NUMBER];
		for(int i = 0; i < Rules::instance()->getPegs(); i++) {
			hidden_code[i] = static_cast<unsigned char>(remaining_colors * (qrand()/(RAND_MAX + 1.0)));
			if (!Rules::instance()->isSameColor()) {
				for(int j = 0; j < i; j++)
					if (hidden_code[j] <= hidden_code[i])
						hidden_code[i]++;
				remaining_colors--;
			}
			createPegForBox(masterBoxes.at(i), hidden_code[i]);
			masterBoxes.at(i)->setState(Box::State::None);
		}

	Guess::instance()->setCode(hidden_code);
	setNextRowInAction();
	showMessage();
	showInformation();
	state = State::WaittingFirstRowFill;
	okButton->setPos(pinBoxes.at(playedMoves)->pos() + QPoint(0, 1));
	// from now on the onPinBoxPressed function continues the game, after the code row is filled
}

void Game::resizeEvent(QResizeEvent *event)
{
	fitInView(sceneRect(), Qt::KeepAspectRatio);
	QGraphicsView::resizeEvent(event);
}

void Game::showInformation()
{
	if (Rules::instance()->getMode() == Mode::MVH) {
		if (Guess::instance()->possibles == 1)
		{
			information->setText(tr("The Code Is Cracked!"));
		} else if (Guess::instance()->possibles > 10000) {
			information->setText(QString("%1    %2: %3").arg(tr("Random Guess")).
								  arg(tr("Remaining")).arg(Board::instance()->getLoale()->toString(Guess::instance()->possibles)));
		} else {
			switch (Guess::instance()->algorithm) {
			case Algorithm::MostParts:
				information->setText(QString("%1: %2    %3: %4").arg(tr("Most Parts")).
										  arg(Board::instance()->getLoale()->toString(Guess::instance()->weight)).arg(tr("Remaining")).
										  arg(Board::instance()->getLoale()->toString(Guess::instance()->possibles)));
				break;
			case Algorithm::WorstCase:
				information->setText(QString("%1: %2    %3: %4").arg(tr("Worst Case")).
										  arg(Board::instance()->getLoale()->toString(Guess::instance()->weight)).arg(tr("Remaining")).
										  arg(Board::instance()->getLoale()->toString(Guess::instance()->possibles)));
				break;
			default:
				information->setText(QString("%1: %2    %3: %4").arg(tr("Expected Size")).
										  arg(Board::instance()->getLoale()->toString(Guess::instance()->weight)).arg(tr("Remaining")).
										  arg(Board::instance()->getLoale()->toString(Guess::instance()->possibles)));
				break;
			}

		}
	} else {
		information->setText(QString("%1: %2   %3: %4   %5: %6").arg(tr("Slots", "", Rules::instance()->getPegs())).
								  arg(Board::instance()->getLoale()->toString(Rules::instance()->getPegs())).arg(tr("Colors", "", Rules::instance()->getColors())).
								  arg(Board::instance()->getLoale()->toString(Rules::instance()->getColors())).arg(tr("Same Colors")).
							 arg(Rules::instance()->isSameColor() ? tr("Yes"): tr("No")));
	}
}

void Game::showMessage()
{
	bool is_MVH = (Rules::instance()->getMode() == Mode::MVH);
	switch (state) {
	case State::Win:
		if (is_MVH)
			message->setText(tr("Success! I Win"));
		else
			message->setText(tr("Success! You Win"));
		break;
	case State::Lose:
		if (is_MVH)
			message->setText(tr("Game Over! I Failed"));
		else
			message->setText(tr("Game Over! You Failed"));
		break;
	case State::Resign:
		message->setText(tr("You Resign"));
		break;
	case State::Thinking:
		message->setText(tr("Let Me Think"));
		break;
	case State::WaittingPinboxPress:
		message->setText(tr("Press OK"));
		break;
	case State::WaittingOkButtonPress:
		message->setText(tr("Please Put Your Pins And Press OK"));
		break;
	case State::WaittingDoneButtonPress:
		message->setText(tr("Press Done"));
		break;
	default:
		message->setText(tr("Place Your Pegs"));
		break;
	}
}
void Game::drawBackground(QPainter *painter, const QRectF &rect)
{
	painter->fillRect(rect, QColor(200, 200, 200));// set scene background color
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
	painter->setBrush(QBrush(QColor(150, 150, 150)));
	painter->drawRect(QRectF(4, 129, 318, 400));

	QLinearGradient bot_grad(0, 530, 0, 557);
	bot_grad.setColorAt(0.0, QColor(204, 204, 204));
	bot_grad.setColorAt(0.3, QColor(206, 206, 206));
	bot_grad.setColorAt(1.0, QColor(180, 180, 180));
	painter->setBrush(QBrush(bot_grad));
	painter->drawRect(QRect(1, 529, 318, 28));
	painter->setBrush(QBrush(QColor(239, 239, 239)));
	painter->setClipping(false);

	QLinearGradient frame_grad(0, 190, 320, 370);
	frame_grad.setColorAt(0.0, QColor(240, 240, 240));
	frame_grad.setColorAt(0.49, QColor(240, 240, 240));
	frame_grad.setColorAt(0.50, QColor(80, 80, 80));
	frame_grad.setColorAt(1.0, QColor(80, 80, 80));
	QPen frame_pen = QPen(QBrush(frame_grad), 1.5);
	QRectF right_shadow(3.5, 3.5, 313, 553);
	painter->setBrush(Qt::NoBrush);
	painter->setPen(frame_pen);
	painter->drawRoundedRect(right_shadow, 9.8, 9.8);

	QLinearGradient sol_frame_grad = QLinearGradient(50, 70, 50, 110);
	sol_frame_grad.setColorAt(0.0, QColor(80, 80, 80));
	sol_frame_grad.setColorAt(1.0, QColor(255, 255, 255));
	QPen sol_frame_pen(QBrush(sol_frame_grad), 1);

	painter->setPen(sol_frame_pen);

	QRectF sol_container(41, 68, 235, 42);
	QRectF sol_frame(42, 69, 235, 41.5);

	painter->drawRoundedRect(sol_container, 21,21);
	painter->setBrush(QColor(150, 150, 150));
	painter->drawRoundedRect(sol_frame, 20, 20);
	painter->setRenderHint(QPainter::TextAntialiasing, true);
}
