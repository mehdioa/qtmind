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

class Peg;
class PinBox;
class PegBox;
class Button;
class Solver;
class Message;
class QSoundEffect;

class Board: public QGraphicsView
{
	Q_OBJECT

public:
	explicit Board(QWidget *parent = 0);
	~Board();
	void play();
	void autoPutPinsCloseRow(const bool &set_pins, const bool &close_row);
	void reset(const int &peg_n, const int &color_n, const GameMode &mode_n,
			   const bool &samecolor, const Algorithm &algorithm_n,
			   const bool &set_pins, const bool &close_row, QLocale *locale_n,
			   const bool &show_colors, const bool &show_indicators,
			   const IndicatorType &indicator_n);
	GameState getState() const {return mGameState;}
	void setAlgorithm(const Algorithm &algorithm_n);

protected:
	void drawBackground(QPainter *painter, const QRectF &rect);
	void resizeEvent(QResizeEvent *event);

signals:
	void showIndicatorsSignal(bool, bool, IndicatorType);
	void startGuessingSignal(Algorithm);
	void resetGameSignal(int peg_no, int color_no, bool allow_same_color);
	void interuptSignal();

private slots:
	void onPegMouseReleased(Peg *);
	void onPegMouseDoubleClicked(Peg *);
	void onOkButtonPressed();
	void onDoneButtonPressed();
	void onPinBoxPressed();
	void onShowIndicators(bool show_colors, bool show_indicators, IndicatorType);
	void onRevealOnePeg();
	void onResigned();
	void onGuessReady(const Algorithm &alg, const QString &guess,
					  const int &possibleSize, const qreal &lastWeight);
	void onPreferencesChanged();

private:
	void playCodeMaster();
	void playCodeBreaker();
	void createBoxes();
	void createPegForBox(PegBox *box, int color, bool underneath = false,
						 bool plain = false);// backPeg is used to put an extra peg under initial pegs,
	void codeRowFilled(const bool &filled);
	void showTranslatedInformation(const Algorithm &alg, const int &possibleSize, const qreal &minWeight);
	void initializeScene();

private:
	QList<PinBox *> mPinBoxes;	//	black-white pins
	QList<PegBox *> mPegBoxes;	//	right boxes that contains color-pegs to put on codeboxes
	QList<PegBox *> mCodeBoxes;	//	middle boxes that is filled by a player
	QList<PegBox *> mCurrentBoxes;	//	the active row of codeboxes
	QList<PegBox *> mMasterBoxes;	//	the mastercode boxes

	GameState mGameState;
	GameMode mGameMode;
	int mPegNumber;
	int mColorNumber;
	bool mSameColorAllowed;
	bool mAutoPutPins;
	bool mAutoCloseRows;
	bool mDone;
	int mFontSize;
	QString mFontName;
	Algorithm mAlgorithm;
	bool mShowColors;
	bool mShowIndicators;
	IndicatorType mIndicatorType;
	QString mMasterCode;
	QString mGuess;
	QLocale *mLocale;
	Solver *mSolver;
	Button *mOkButton;
	Button *mDoneButton;
	Message *mMessage;
	Message *mInformation;

#if QT_VERSION >= 0x050000
	QSoundEffect *pegDropSound;
	QSoundEffect *pegDropRefuseSound;
	QSoundEffect *buttonPressSound;
#endif
};

#endif // BOARD_H
