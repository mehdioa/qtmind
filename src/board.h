/***********************************************************************
 *
 * Copyright (C) 2013 Mehdi Omidal <mehdioa@gmail.com>
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
#include "peg.h"
#include <QGraphicsView>

class Peg;
class EmptyBox;
class PinBox;
class PegBox;
class Button;
class Game;
class Message;

class Board: public QGraphicsView
{
	Q_OBJECT

public:
	Board(QWidget* parent = 0);
	~Board();
	void handleButtonRelease(const QString &);
	void generate();
	void play(const int &mode);
	void setPinsRow(const bool &set_pins, const bool &closeRow);
	void reset(const int& peg_n, const int& color_n, const int &mode_n,
			   const bool &samecolor, const int &algorithm_n, const bool &set_pins,
			   const bool &close_row, const int& indicator_n = TYPE_NONE);

protected:
	void drawBackground(QPainter* painter, const QRectF& rect);
//	void drawForeground(QPainter* painter, const QRectF& rect);
	void resizeEvent(QResizeEvent* event);
	void setIndicatorType(const int indicator_n);
	void setAlgorithmType(const int& algorithm_n);

signals:
	void changePegIndicator(const INDICATOR_TYPE &indicator_t);

private slots:
	void onPegMouseRelease(QPoint position, int color);
	void onOkButtonPressed();
	void onDoneButtonPressed();
	void onPinBoxPressed();
	void onChangeIndicators(const int &indicator_n);
	void onChangeMode(const int&);

private:
	void playCodeMaster();
	void playCodeBreaker();
	void createBoxes();
//	void getCode();
	void createPegForBox(PegBox* box, int color, bool backPeg = false);// backPeg should only be used to put an extra peg under initial pegs,
	void codeRowFilled(bool filled);


	QList<PinBox*> mPinBoxes;
	QList<PegBox*> mPegBoxes;
	QList<PegBox*> mCodeBoxes;
	QList<PegBox*> mCurrentBoxes;	//	boxes that can be filled by player
	QList<PegBox*> mMasterBoxes;

	GAME_STATE mState;
	GAME_MODE mMode;
	int mPegNumber;
	int mColorNumber;
	Algorithm mAlgorithm;
	bool mSameColor;
	bool mSetPins;
	bool mCloseRow;
	bool mDone;
	INDICATOR_TYPE mIndicator;
	Game* mGame;
	Button* mOkButton;
	Button* mDoneButton;
	Message* mMessage;
	QString mMasterCode;
	QString mGuess;

//	bool rowFillState;
};

#endif // BOARD_H
