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

#include <QGraphicsView>

class Peg;
class PinBox;
class PegBox;
class Button;
class Solver;
class Message;

/**
 * @brief A class to represent the game logic. It is the model in
 * the MVC pattern of the game. All the classes Peg, Box, PinBox,
 * Button, Solver, Message, and Guess are parts of this class.
 * It is the heart of the game. It is responssible for the graphics
 * of the game and is the class that mainwindow interact with.
 *
 */
class Game: public QGraphicsView
{
	Q_OBJECT

public:
	/**
	 * @brief Game create a game with board
	 * @param parent the parent of the game
	 */
	explicit Game();
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
	void createPegForBox(PegBox *_box, int _color);
	PegBox *createPegBox(const QPoint &_position);
	Peg *createPeg(const QPointF &_position, const int &_color);
	Peg *createPeg(PegBox *_box, const int &_color);
	void codeRowFilled(const bool &_filled);
	void showInformation();
	void showMessage();
	void initializeScene();
	void freezeScene();
	void setNextRowInAction();
	void getNextGuess();
	Game::Player winner() const;

private:

	QList<PinBox *> m_pinBoxes;		/**< black-white pins */
	QList<PegBox *> m_pegBoxes;		/**< right boxes that contains color-pegs to put on codeboxes*/
	QList<PegBox *> m_codeBoxes;	    /**< middle boxes that is filled by a player*/
	QList<PegBox *> m_currentBoxes;	/**< the active row of codeboxes */
	QList<PegBox *> m_masterBoxes;	/**< the mastercode boxes */

	Game::State m_state;              /**< TODO */
	Solver *m_solver;                 /**< TODO */
	Button *m_okButton;               /**< TODO */
	Button *m_doneButton;             /**< TODO */
	Message *m_message;               /**< TODO */
	Message *m_information;			/**< TODO */
	int m_movesPlayed;                /**< TODO */
};

#endif // GAME_H
