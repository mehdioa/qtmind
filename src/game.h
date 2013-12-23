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
#include <QGraphicsView>
#include <QLocale>

enum class GameMode {
	Codemaker,		//HUMAN BREAKS THE CODE
	Codebreaker		//MACHINE BREAKS THE CODE
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

class Game: public QGraphicsView
{
	Q_OBJECT

public:
	explicit Game(QWidget *parent = 0);
	~Game();
	void play();
	void autoPutPinsCloseRow(const bool &set_pins, const bool &close_row);
	void reset(const int &peg_n, const int &color_n, const GameMode &mode_n,
			   const bool &samecolor, const Algorithm &algorithm_n,
			   const bool &set_pins, const bool &close_row, QLocale *locale_n,
			   const bool &show_colors, const bool &show_indicators,
			   const IndicatorType &indicator_n);
	GameState getState() const {return gameState;}
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
	void onGuessReady(const Algorithm &alg, const QString &m_guess,
					  const int &m_possibleSize, const qreal &m_lastWeight);
	void onPreferencesChanged();

private:
	void playCodeMaster();
	void playCodeBreaker();
	void createBoxes();
	void createPegForBox(PegBox *m_box, int m_color, bool m_underneath = false,
						 bool m_plain = false);// backPeg is used to put an extra peg under initial pegs,
	void codeRowFilled(const bool &m_filled);
	void showTranslatedInformation(const Algorithm &m_algorithm, const int &m_possibleSize, const qreal &m_minWeight);
	void initializeScene();

private:
	QList<PinBox *> pinBoxes;	//	black-white pins
	QList<PegBox *> pegBoxes;	//	right boxes that contains color-pegs to put on codeboxes
	QList<PegBox *> codeBoxes;	//	middle boxes that is filled by a player
	QList<PegBox *> currentBoxes;	//	the active row of codeboxes
	QList<PegBox *> masterBoxes;	//	the mastercode boxes

	GameState gameState;
	GameMode gameMode;
	int pegNumber;
	int colorNumber;
	bool sameColorAllowed;
	bool autoPutPins;
	bool autoCloseRows;
	bool done;
	int fontSize;
	QString fontName;
	Algorithm algorithm;
	bool showColors;
	bool showIndicators;
	IndicatorType indicatorType;
	QString masterCode;
	QString guess;
	QLocale *locale;
	Solver *solver;
	Button *okButton;
	Button *doneButton;
	Message *message;
	Message *information;

#if QT_VERSION >= 0x050000
	QSoundEffect *pegDropSound;
	QSoundEffect *pegDropRefuseSound;
	QSoundEffect *buttonPressSound;
#endif
};

#endif // GAME_H
