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
	m_state(State::None),
	m_solver(0)
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
	if (m_solver) {
		m_solver->interupt();
		m_solver->quit();
		m_solver->wait();
		m_solver->deleteLater();
	}

	scene()->clear();
}

void Game::codeRowFilled(const bool &_filled)
{
	if(Rules::instance()->m_mode == Mode::HVM) {
		m_okButton->setVisible(_filled);
		m_okButton->setEnabled(_filled);
		if (_filled) {
			m_state = State::WaittingPinboxPress;
			showMessage();

			if (Board::instance()->m_autoCloseRows)
				onOkButtonPressed();
		} else {
			if (m_movesPlayed == 0)
				m_state = State::WaittingFirstRowFill;
			else
				m_state = State::WaittingCodeRowFill;

			showMessage();
		}
	} else {	//Rules::instance()->m_mode == Mode::MVH
		m_doneButton->setVisible(_filled);

		if (_filled)//the user is not done putting the master code
			m_state = State::WaittingDoneButtonPress;
		else
			m_state = State::WaittingHiddenCodeFill;

		showMessage();
	}
}

void Game::createBoxes()
{
	/*	This function creates boxes and put them in QLists. They are
	 *	stored in QList from bottom to top and left to right.
	 */
	m_codeBoxes.clear();
	m_pinBoxes.clear();
	m_pegBoxes.clear();
	m_currentBoxes.clear();
	m_masterBoxes.clear();

	QPoint left_bottom_corner(4, 489);

	for (int i = 0; i < MAX_COLOR_NUMBER; ++i) {
		QPoint position = left_bottom_corner - QPoint(0, i*40);

		auto pinbox = new PinBox(Rules::instance()->m_pegs, position);
		scene()->addItem(pinbox);
		m_pinBoxes.append(pinbox);

		position.setX(160-20*Rules::instance()->m_pegs);

		for (int j = 0; j < Rules::instance()->m_pegs; ++j) {
			m_codeBoxes.append(createPegBox(position+QPoint(j*40, 0)));
		}

		position.setX(277);	//go to right corner for the peg boxes

		PegBox *pegbox = createPegBox(position);
		m_pegBoxes.append(pegbox);
		if (i < Rules::instance()->m_colors) {
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
	for (int i = 0; i < Rules::instance()->m_pegs; ++i) {
		m_masterBoxes.append(createPegBox(QPoint(160-20*Rules::instance()->m_pegs+i*40, 70)));
	}
}

void Game::createPegForBox(PegBox *_box, int _color)
{
	QPointF pos = _box->sceneBoundingRect().center();
	if (_box->hasPeg()) {
		_box->setPegColor(_color);
	} else {
		Peg *peg = createPeg(pos, _color);
		_box->setPeg(peg);
		connect(peg, SIGNAL(mouseReleaseSignal(Peg *)), this, SLOT(onPegMouseReleased(Peg *)));
		connect(peg, SIGNAL(mouseDoubleClickSignal(Peg*)), this, SLOT(onPegMouseDoubleClicked(Peg*)));
		connect(this, SIGNAL(showIndicatorsSignal()), peg, SLOT(onShowIndicators()));
	}
}

PegBox *Game::createPegBox(const QPoint &_position)
{
	auto pegbox = new PegBox(_position);
	scene()->addItem(pegbox);
	return pegbox;
}

Peg *Game::createPeg(const QPointF &_position, const int &_color)
{
	auto peg = new Peg(_position, _color);
	scene()->addItem(peg);
	return peg;
}

Peg *Game::createPeg(PegBox *_box, const int &_color)
{
	return createPeg(_box->sceneBoundingRect().center(), _color);
}

void Game::freezeScene()
{
	setStateOfList(&m_masterBoxes, Box::State::Past);
	setStateOfList(&m_currentBoxes, Box::State::Past);
	m_pinBoxes.at(m_movesPlayed)->setState(Box::State::Past);
	setStateOfList(&m_pegBoxes, Box::State::Future);
}

void Game::setNextRowInAction()
{
	m_currentBoxes.clear();
	for(int i = m_movesPlayed*Rules::instance()->m_pegs; i < (m_movesPlayed+1)*Rules::instance()->m_pegs; ++i)
		m_currentBoxes.append(m_codeBoxes.at(i));
	setStateOfList(&m_currentBoxes, Box::State::Current);
	m_state = State::WaittingCodeRowFill;
}

void Game::getNextGuess()
{
	m_state = State::Thinking;
	m_solver->startGuessing();
}

Game::Player Game::winner() const
{
	int blacks, whites;
	m_pinBoxes.at(m_movesPlayed)->getValue(blacks, whites);
	if (blacks == Rules::instance()->m_pegs)
		return Player::CodeBreaker;
	else if (m_movesPlayed >= MAX_COLOR_NUMBER - 1)
		return Player::CodeMaker;
	else
		return Player::None;
}

void Game::initializeScene()
{
	m_codeBoxes.clear();
	m_pinBoxes.clear();
	m_pegBoxes.clear();
	m_masterBoxes.clear();
	m_currentBoxes.clear();
	m_movesPlayed = 0;

	scene()->clear();
	setInteractive(true);

	m_okButton = new Button(36, tr("OK"));
	scene()->addItem(m_okButton);
	m_okButton->setZValue(2);
	connect(m_okButton, SIGNAL(buttonPressed()), this, SLOT(onOkButtonPressed()));
	m_okButton->setEnabled(false);
	m_okButton->setVisible(false);

	m_doneButton = new Button(158, tr("Done"));
	m_doneButton->setPos(79, 118);
	m_doneButton->setVisible(false);
	m_doneButton->setZValue(2);
	m_doneButton->setEnabled(false);
	connect(m_doneButton, SIGNAL(buttonPressed()), this, SLOT(onDoneButtonPressed()));
	scene()->addItem(m_doneButton);

	m_message = new Message("#303030");
	scene()->addItem(m_message);
	m_message->setPos(20, 0);

	m_information = new Message("#808080", 4);
	scene()->addItem(m_information);
	m_information->setPos(20, 506);
	showInformation();
	createBoxes();
	scene()->update();
}

void Game::onPegMouseReleased(Peg *peg)
{
	QPointF position = peg->sceneBoundingRect().center();
	int color = peg->getColor();
	//if same color is not allowed and there is already a color-peg visible, we just ignore drop
	if(!Rules::instance()->m_sameColors) {
		foreach(PegBox *box, m_currentBoxes) {
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

	foreach(PegBox *box, m_currentBoxes) {
		if (box->sceneBoundingRect().contains(position) && dropOnlyOnce) {
			dropOnlyOnce = false;
			createPegForBox(box, color);
			Board::instance()->play(Board::Sound::PegDrop);
			box->setPegState(Peg::State::Visible);
		}//	end if

		if (box->getPegState() != Peg::State::Visible)
			newRowFillState = false;
	} //	end foreach

	//	if (master) code row state changed, go to codeRowFilled
	if (newRowFillState != rowFillState) {
		rowFillState = newRowFillState;
		codeRowFilled(rowFillState);
	}
}

void Game::onPegMouseDoubleClicked(Peg *peg)
{
	foreach(PegBox *box, m_currentBoxes) {
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
	if(m_okButton) {
		m_okButton->setLabel(tr("OK"));
		m_okButton->update();
	}
	if (m_doneButton) {
		m_doneButton->setLabel(tr("Done"));
		m_doneButton->update();
	}
	showInformation();
	showMessage();
}

bool Game::isRunning()
{
	switch (m_state) {
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
	if (Rules::instance()->m_mode == Mode::HVM && isRunning()) {
		foreach(PegBox *box, m_masterBoxes) {
			if(box->getState() != Box::State::Past) {
				if (box == m_masterBoxes.last()) {
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
	if (Rules::instance()->m_mode == Mode::HVM && isRunning()) {
		m_state = State::Resign;
		showMessage();
		freezeScene();
	}
}

void Game::onOkButtonPressed()
{
	if(Rules::instance()->m_mode == Mode::MVH) {
		int blacks, whites;
		m_pinBoxes.at(m_movesPlayed)->getValue(blacks, whites);
		if(!m_solver->setResponse(blacks, whites)) {
			m_message->setText(tr("Not Possible, Try Again"));
			return;
		}
		Board::instance()->play(Board::Sound::ButtonPress);
		m_okButton->setVisible(false);

		m_pinBoxes.at(m_movesPlayed)->setState(Box::State::Past);

		switch (winner()) {
		case Player::CodeBreaker:
			m_state = State::Win;
			freezeScene();
			break;
		case Player::CodeMaker:
			m_state = State::Lose;
			freezeScene();
			break;
		default:
			++m_movesPlayed;
			getNextGuess();
			break;
		}

		showMessage();
	} else {
		Board::instance()->play(Board::Sound::ButtonPress);
		m_state = State::Running;

		unsigned char new_guess[MAX_SLOT_NUMBER];
		for(int i = 0; i < Rules::instance()->m_pegs; ++i)
			new_guess[i] = m_currentBoxes.at(i)->getPegColor();
		Guess::instance()->setGuess(new_guess);

		m_pinBoxes.at(m_movesPlayed)->setPins();
		m_pinBoxes.at(m_movesPlayed)->setState(Box::State::Past);

		setStateOfList(&m_currentBoxes, Box::State::Past);
		m_currentBoxes.clear();

		switch (winner()) {
		case Player::CodeBreaker:
			m_state = State::Win;
			freezeScene();
			break;
		case Player::CodeMaker:
			m_state = State::Lose;
			freezeScene();
			break;
		default:
			++m_movesPlayed;
			setNextRowInAction();
			break;
		}

		showMessage();
		m_okButton->setVisible(false);
		m_okButton->setPos(m_pinBoxes.at(m_movesPlayed)->pos() + QPoint(0, 1));
	}
}

void Game::onDoneButtonPressed()
{
	Board::instance()->play(Board::Sound::ButtonPress);
	m_state = State::Running;

	m_doneButton->setVisible(false);
	m_doneButton->setEnabled(false);

	setStateOfList(&m_currentBoxes, Box::State::Past);
	setStateOfList(&m_pegBoxes, Box::State::Future);

	unsigned char new_guess[MAX_SLOT_NUMBER];
	for(int i = 0; i < Rules::instance()->m_pegs; ++i)
		new_guess[i] = m_currentBoxes.at(i)->getPegColor();
	Guess::instance()->setCode(new_guess);

	m_currentBoxes.clear();
	getNextGuess();
	showMessage();
}

void Game::onGuessReady()
{
	m_state = State::Running;
	showInformation();

	int box_index = m_movesPlayed*Rules::instance()->m_pegs;
	for(int i = 0; i < Rules::instance()->m_pegs; ++i) {
		createPegForBox(m_codeBoxes.at(box_index + i), Guess::instance()->m_guess[i]);
		m_codeBoxes.at(box_index + i)->setState(Box::State::Past);
	}
	m_state = State::WaittingOkButtonPress;
	showMessage();
	m_pinBoxes.at(m_movesPlayed)->setState(Box::State::None);
	m_okButton->setEnabled(true);
	m_okButton->setVisible(true);
	m_okButton->setPos(m_pinBoxes.at(m_movesPlayed)->pos()-QPoint(0, 39));

	if (Board::instance()->m_autoPutPins)
		m_pinBoxes.at(m_movesPlayed)->setPins();
}

void Game::play()
{
	stop();

	if(Rules::instance()->m_mode == Mode::MVH)
		playMVH();
	else // (Rules::instance()->m_mode == Mode::HVM)
		playHVM();
}

void Game::stop()
{
	if (m_solver) {
		m_solver->interupt();
		m_solver->quit();
		m_solver->wait();
	}
	initializeScene();
	m_state = State::None;
}

void Game::playMVH()
{
	m_doneButton->setZValue(2);
	m_doneButton->setVisible(false);
	m_doneButton->setEnabled(true);

	if (!m_solver) {
		m_solver = new Solver(this);
		connect(m_solver, SIGNAL(guessDoneSignal()), this, SLOT(onGuessReady()));

	}
	m_solver->interupt();
	m_solver->reset();

	m_state = State::WaittingHiddenCodeFill;
	showMessage();
	showInformation();

	//initializing currentrow
	for(int i = 0; i < Rules::instance()->m_pegs; ++i) {
		m_currentBoxes.append(m_masterBoxes.at(i));
		m_masterBoxes.at(i)->setState(Box::State::Current);
	}

	/*	Nothing happening from here till the user fill the master code
	 *	row and press the done button. After the done button is pressed,
	 *	the onm_doneButtonPressed is continuing the game
	 */
}

void Game::playHVM()
{
	qsrand(time(NULL));
	int remaining_colors = Rules::instance()->m_colors;
	unsigned char hidden_code[MAX_SLOT_NUMBER];
		for(int i = 0; i < Rules::instance()->m_pegs; i++) {
			hidden_code[i] = static_cast<unsigned char>(remaining_colors * (qrand()/(RAND_MAX + 1.0)));
			if (!Rules::instance()->m_sameColors) {
				for(int j = 0; j < i; j++)
					if (hidden_code[j] <= hidden_code[i])
						hidden_code[i]++;
				remaining_colors--;
			}
			createPegForBox(m_masterBoxes.at(i), hidden_code[i]);
			m_masterBoxes.at(i)->setState(Box::State::None);
		}

	Guess::instance()->setCode(hidden_code);
	setNextRowInAction();
	showMessage();
	showInformation();
	m_state = State::WaittingFirstRowFill;
	m_okButton->setPos(m_pinBoxes.at(m_movesPlayed)->pos() + QPoint(0, 1));
	// from now on the onPinBoxPressed function continues the game, after the code row is filled
}

void Game::resizeEvent(QResizeEvent *event)
{
	fitInView(sceneRect(), Qt::KeepAspectRatio);
	QGraphicsView::resizeEvent(event);
}

void Game::showInformation()
{
	if (Rules::instance()->m_mode == Mode::MVH) {
		if (Guess::instance()->m_possibles == 1)
		{
			m_information->setText(tr("The Code Is Cracked!"));
		} else if (Guess::instance()->m_possibles > 10000) {
			m_information->setText(QString("%1    %2: %3").arg(tr("Random Guess")).
								  arg(tr("Remaining")).arg(Board::instance()->m_locale.toString(Guess::instance()->m_possibles)));
		} else {
			switch (Guess::instance()->m_algorithm) {
			case Algorithm::MostParts:
				m_information->setText(QString("%1: %2    %3: %4").arg(tr("Most Parts")).
										  arg(Board::instance()->m_locale.toString(Guess::instance()->m_weight)).arg(tr("Remaining")).
										  arg(Board::instance()->m_locale.toString(Guess::instance()->m_possibles)));
				break;
			case Algorithm::WorstCase:
				m_information->setText(QString("%1: %2    %3: %4").arg(tr("Worst Case")).
										  arg(Board::instance()->m_locale.toString(Guess::instance()->m_weight)).arg(tr("Remaining")).
										  arg(Board::instance()->m_locale.toString(Guess::instance()->m_possibles)));
				break;
			default:
				m_information->setText(QString("%1: %2    %3: %4").arg(tr("Expected Size")).
										  arg(Board::instance()->m_locale.toString(Guess::instance()->m_weight)).arg(tr("Remaining")).
										  arg(Board::instance()->m_locale.toString(Guess::instance()->m_possibles)));
				break;
			}

		}
	} else {
		m_information->setText(QString("%1: %2   %3: %4   %5: %6").arg(tr("Slots", "", Rules::instance()->m_pegs)).
								  arg(Board::instance()->m_locale.toString(Rules::instance()->m_pegs)).arg(tr("Colors", "", Rules::instance()->m_colors)).
								  arg(Board::instance()->m_locale.toString(Rules::instance()->m_colors)).arg(tr("Same Colors")).
							 arg(Rules::instance()->m_sameColors ? tr("Yes"): tr("No")));
	}
}

void Game::showMessage()
{
	bool is_MVH = (Rules::instance()->m_mode == Mode::MVH);
	switch (m_state) {
	case State::Win:
		if (is_MVH)
			m_message->setText(tr("Success! I Win"));
		else
			m_message->setText(tr("Success! You Win"));
		break;
	case State::Lose:
		if (is_MVH)
			m_message->setText(tr("Game Over! I Failed"));
		else
			m_message->setText(tr("Game Over! You Failed"));
		break;
	case State::Resign:
		m_message->setText(tr("You Resign"));
		break;
	case State::Thinking:
		m_message->setText(tr("Let Me Think"));
		break;
	case State::WaittingPinboxPress:
		m_message->setText(tr("Press OK"));
		break;
	case State::WaittingOkButtonPress:
		m_message->setText(tr("Please Put Your Pins And Press OK"));
		break;
	case State::WaittingDoneButtonPress:
		m_message->setText(tr("Press Done"));
		break;
	default:
		m_message->setText(tr("Place Your Pegs"));
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
