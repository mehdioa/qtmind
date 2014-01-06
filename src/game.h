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

#ifndef GAME_H
#define GAME_H

#include "constants.h"
#include "guesselement.h"
#include <QGraphicsView>

enum class GameState
{
	None,
	Running,
	Win,
	Lose,
	WaittingFirstRowFill,
};

class Peg;
class PinBox;
class PegBox;
class Button;
class Solver;
class Message;
class GameRules;
class BoardAid;

class Game: public QGraphicsView
{
	Q_OBJECT

public:
	explicit Game(GameRules *game_rules, BoardAid *board_aid, QWidget *parent = 0);
	~Game();
	void play();
	void stop();
	GameState getState() const {return gameState;}
	void setAlgorithm(const Algorithm &algorithm_n);
	void changeIndicators();
	void retranslateTexts();

protected:
	void drawBackground(QPainter *painter, const QRectF &rect);
	void resizeEvent(QResizeEvent *event);

signals:
	void showIndicatorsSignal();
	void startGuessingSignal();
	void resetGameSignal();
	void interuptSignal();

private slots:
	void onPegMouseReleased(Peg *);
	void onPegMouseDoubleClicked(Peg *);
	void onOkButtonPressed();
	void onDoneButtonPressed();
	void onPinBoxPressed();
	void onRevealOnePeg();
	void onResigned();
	void onGuessReady();

private:
	void playMVH();
	void playHVM();
	void createBoxes();
	void createPegForBox(PegBox *m_box, int m_color);
	Peg *createPeg(PegBox *m_box, const int &m_color);
	void codeRowFilled(const bool &m_filled);
	void showTranslatedInformation();
	void initializeScene();
	void freezeAllLists();
	void setNextRowInAction();

private:
	QList<PinBox *> pinBoxes;	//	black-white pins
	QList<PegBox *> pegBoxes;	//	right boxes that contains color-pegs to put on codeboxes
	QList<PegBox *> codeBoxes;	//	middle boxes that is filled by a player
	QList<PegBox *> currentBoxes;	//	the active row of codeboxes
	QList<PegBox *> masterBoxes;	//	the mastercode boxes

	GameState gameState;
	GameRules *gameRules;
	BoardAid *boardAid;
	GuessElement guessElement;
	Solver *solver;
	Button *okButton;
	Button *doneButton;
	Message *message;
	Message *information;
	int playedMoves;
};

#endif // GAME_H
