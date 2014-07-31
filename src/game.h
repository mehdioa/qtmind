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

#include "guess.h"
#include <QGraphicsView>

class Peg;
class PinBox;
class PegBox;
class Button;
class Solver;
class Message;
class Rules;
class Board;

/**
 * @brief A class to represent the game logic. It is the heart of the game.
 * It is responssible for the graphics of the game and is the class that
 * mainwindow interact with.
 *
 */
class Game: public QGraphicsView
{
	Q_OBJECT

public:
	/**
	 * @brief Game create a game with rules and board
	 * @param game_rules the rules of the game
	 * @param board_aid the board of the game
	 * @param parent the parent of the game
	 */
	explicit Game(Rules *game_rules, Board *board_aid, QWidget *parent = 0);
	~Game();

	/**
	 * @brief play start playing
	 */
	void play();

	/**
	 * @brief stop stop playing
	 */
	void stop();

	/**
	 * @brief setAlgorithm change algorithm
	 * @param algorithm_n the algorithm
	 */
	void setAlgorithm(const Rules::Algorithm &algorithm_n);

	/**
	 * @brief changeIndicators change indicators of the pegs
	 */
	void changeIndicators();

	/**
	 * @brief retranslateTexts retranslate the game
	 */
	void retranslateTexts();

	/**
	 * @brief isRunning is the game running?
	 * @return true if the game is running, false otherwise
	 */
	bool isRunning();

protected:
	/**
	 * @brief drawBackground
	 * @param painter
	 * @param rect
	 */
	void drawBackground(QPainter *painter, const QRectF &rect);
	/**
	 * @brief resizeEvent
	 * @param event
	 */
	void resizeEvent(QResizeEvent *event);

signals:
	/**
	 * @brief showIndicatorsSignal
	 */
	void showIndicatorsSignal();
	/**
	 * @brief startGuessingSignal
	 */
	void startGuessingSignal();
	/**
	 * @brief resetGameSignal
	 */
	void resetGameSignal();
	/**
	 * @brief interuptSignal
	 */
	void interuptSignal();

protected slots:
	void onPegMouseReleased(Peg *);
	void onPegMouseDoubleClicked(Peg *);
	void onOkButtonPressed();
	void onDoneButtonPressed();
	void onRevealOnePeg();
	void onResigned();
	void onGuessReady();

private:

	enum class Player {
		CodeMaker,
		CodeBreaker,
		None
	};

	enum class State {
		None,
		Running,
		Win,
		Lose,
		Resign,
		Thinking,
		WaittingFirstRowFill,
		WaittingCodeRowFill,
		WaittingHiddenCodeFill,
		WaittingPinboxPress,
		WaittingOkButtonPress,
		WaittingDoneButtonPress
	};

	void playMVH();
	void playHVM();
	void createBoxes();
	void createPegForBox(PegBox *m_box, int m_color);
	PegBox *createPegBox(const QPoint &m_position);
	Peg *createPeg(const QPointF &m_position, const int &m_color);
	Peg *createPeg(PegBox *m_box, const int &m_color);
	void codeRowFilled(const bool &m_filled);
	void showInformation();
	void showMessage();
	void initializeScene();
	void freezeScene();
	void setNextRowInAction();
	void getNextGuess();
	Game::Player winner() const;

private:

	QList<PinBox *> pinBoxes;		/**< black-white pins */
	QList<PegBox *> pegBoxes;		/**< right boxes that contains color-pegs to put on codeboxes*/
	QList<PegBox *> codeBoxes;	    /**< middle boxes that is filled by a player*/
	QList<PegBox *> currentBoxes;	/**< the active row of codeboxes */
	QList<PegBox *> masterBoxes;	/**< the mastercode boxes */

	Game::State state;              /**< TODO */
	Rules *rules;                   /**< TODO */
	Board *board;                   /**< TODO */
	Guess guess;                    /**< TODO */
	Solver *solver;                 /**< TODO */
	Button *okButton;               /**< TODO */
	Button *doneButton;             /**< TODO */
	Message *message;               /**< TODO */
	Message *information;			/**< TODO */
	int playedMoves;                /**< TODO */
};

#endif // GAME_H
