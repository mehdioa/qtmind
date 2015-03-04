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
#include "appinfo.h"
#include "guess.h"

class Peg;
class PinBox;
class PegBox;
class Button;
class Solver;
class Message;
class QLocale;
class Tools;
class Rules;

/**
 * @brief A class to represent the game logic. It is the controler in
 * the MVC pattern of the game. All the classes Peg, Box, PinBox,
 * Button, Solver, Message, and Guess are parts of this class.
 * It is the heart of the game. It is responssible for the graphics
 * of the game and is the class that mainwindow interacts with.
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
	 * @brief retranslateTexts retranslate the game
	 */
	void retranslateTexts();

	/**
	 * @brief isRunning is the game running?
	 * @return true if the game is running, false otherwise
	 */
	bool isRunning();

    void setRules(Rules *rules);

    void setTools(Tools *tools);

public slots:
    /**
     * @brief onFontChanged set the new font and redraw everything
     * @param font_name the font name
     * @param font_size the font size
     */
    void onFontChanged(const QString &font_name, const int &font_size);
    /**
     * @brief changeIndicators change indicators of the pegs
     */
    void onResetIndicators();

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
     * @brief resetIndicatorsSignal notify all the pegs to change their indicators
     */
    void resetIndicatorsSignal();
    /**
     * @brief pegDropSignal notify the sounds class to play the peg drop sound
     */
    void pegDropSignal();
    /**
     * @brief pegDropRefuseSignal notify the sounds class to play the peg drop refuse sound
     */
    void pegDropRefuseSignal();
    /**
     * @brief buttonClickSignal notify the sounds class to play the button click sound
     */
    void buttonClickSignal();

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
    Player winner() const;

private:

    QList<PinBox *> mPinBoxes;		/**< black-white pins */
    QList<PegBox *> mPegBoxes;		/**< right boxes that contains color-pegs to put on codeboxes*/
    QList<PegBox *> mCodeBoxes;	    /**< middle boxes that is filled by a player*/
    QList<PegBox *> mCurrentBoxes;	/**< the active row of codeboxes */
    QList<PegBox *> mMasterBoxes;	/**< the mastercode boxes */

    Game::State mState;              /**< TODO */
    Solver *mSolver;                 /**< TODO */
    Button *mOkButton;               /**< TODO */
    Button *mDoneButton;             /**< TODO */
    Message *mMessage;               /**< TODO */
    Message *mInformation;			/**< TODO */
    int mMovesPlayed;                /**< TODO */
    Guess mGuess;
    Rules *mRules;
    Tools *mTools;
};

#endif // GAME_H
