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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLocale>
#include "constants.h"

class Board;
class QMenu;
class QAction;
class QToolBar;
class QActionGroup;
class QComboBox;
class Board;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void closeEvent(QCloseEvent* event); /** Override parent function to save window geometry. */

private slots:
	void allowSameColorSlot();
	void setPinsCloseRowAutomatically();
	void changeIndicatorsSlot(QAction*);
	void changeGameModeSlot(QAction*);
	void updateNumbersSlot();
	void newGameSlot();
	void onPreferences();
	void about();

signals:
	void changeIndicatorsSignal(const IndicatorType&);

private:
	void setPegsComboBox();
	void setColorsComboBox();
	void setSolvingAlgorithmsComboBox();
	void createBoard();
	void createMenuBar();
	void DrawBoardBackground();

private:

	Board* mBoard;
	GameMode mMode;
	int mColors;
	int mPegs;
	Algorithm mAlgorithm;
	IndicatorType mIndicator;
	bool mSameColorAllowed ;
	bool mSetPins;
	bool mCloseRow;
	QLocale mLocale;
	QString mFontName;
	int mFontSize;

	QAction* allowSameColorAction;
	QAction* setPinsAutomaticallyAction;
	QAction* closeRowAutomaticallyAction;
	QAction* showIndicatorAction;
	QAction* resignAction;

	QComboBox* pegsNumberComboBox;
	QComboBox* colorsNumberComboBox;
	QComboBox* solvingAlgorithmsComboBox;
};

#endif // MAINWINDOW_H
