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

#ifndef BOARD_H
#define BOARD_H

#include "constants.h"
#include <QGraphicsView>
#include <QLocale>

enum class GameMode {
	Master,		//HUMAN BREAKS THE CODE
	Breaker		//MACHINE BREAKS THE CODE
};

class Peg;
class EmptyBox;
class PinBox;
class PegBox;
class Button;
class Game;
class Message;

enum class GameState
{
	None,
	Running,
	Win,
	Lose,
	Resign,
	Guessing,
	WaittingOkButtonPress,
	WaittingDoneButtonPress,
	WaittingFirstRowFill,
	WaittingPinBoxPress
};

class Board: public QGraphicsView
{
	Q_OBJECT

public:
	explicit Board(const QString &font_name = "Sans Serif", const int &font_size = 12, QWidget *parent = 0);
	~Board();
	void play();
	void autoPutPinsCloseRow(const bool &set_pins, const bool &closeRow);
	void reset(const int &peg_n, const int &color_n, const GameMode &mode_n,
			   const bool &samecolor, const Algorithm &algorithm_n,
			   const bool &set_pins, const bool &close_row, QLocale locale_n,
			   const IndicatorType &indicator_n = IndicatorType::None);
	GameState getState() const {return mState;}
	void setAlgorithm(const Algorithm &algorithm_n);

protected:
	void drawBackground(QPainter *painter, const QRectF &rect);
	void resizeEvent(QResizeEvent *event);

signals:
	void IndicatorTypeChangeSignal(IndicatorType);
	void startGuessingSignal(Algorithm);
	void resetGameSignal(int peg_no, int color_no, bool allow_same_color);
	void interuptSignal();

private slots:
	void onPegMouseReleased(const QPoint &position, const int &color);
	void onPegMouseDoubleClicked(Peg *);
	void onOkButtonPressed();
	void onDoneButtonPressed();
	void onPinBoxPressed();
	void onIndicatorTypeChanged(const IndicatorType &indicator_n);
	void onRevealOnePeg();
	void onResigned();
	void onGuessReady(const Algorithm &alg, const QString &guess,
					  const int &possibleSize, const qreal &lastWeight);

private:
	void playCodeMaster();
	void playCodeBreaker();
	void createBoxes();
	void createPegForBox(PegBox *box, int color, bool backPeg = false);// backPeg is used to put an extra peg under initial pegs,
	void codeRowFilled(const bool &filled);
	void setBoxStateOfList(QList<PegBox *> *boxlist, const BoxState &state_t);
	void setBoxStateOfList(QList<PinBox *> *boxlist, const BoxState &state_t);
	void showTranslatedInformation(const Algorithm &alg, const int &possibleSize, const qreal &minWeight);
	void initializeScene();

private:
	QList<PinBox *> mPinBoxes;
	QList<PegBox *> mPegBoxes;
	QList<PegBox *> mCodeBoxes;
	QList<PegBox *> mCurrentBoxes;	//	boxes that can be filled by player
	QList<PegBox *> mMasterBoxes;

	GameState mState;
	GameMode mMode;
	int mPegNumber;
	int mColorNumber;
	bool mSameColor;
	bool mSetPins;
	bool mCloseRow;
	bool mDone;
	Algorithm mAlgorithm;
	IndicatorType mIndicator;
	QString mMasterCode;
	QString mGuess;
	QLocale mLocale;

	QString mFontName;
	int mFontSize;

	Game *mGame;
	Button *mOkButton;
	Button *mDoneButton;
	Message *mMessage;
	Message *mInformation;
};

#endif // BOARD_H
