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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
	void newGameSlot();
	void throwInTheTowelSlot();
	void toggleAllowSameColorSlot();
	void setPinsCloseRowAutomatically();
	void changeIndicatorsSlot(QAction*);
	void doItForMeSlot();
	void changeGameModeSlot(QAction*);
	void about();

signals:
	void changeIndicatorsSignal(const int&);
	void changeModeSignal(const int&);

private:
	void setPegsComboBox();
	void setColorsComboBox();
	void setSolvingAlgorithmsComboBox();

	Board* mBoard;
	int mMode;
	int mColors;
	int mPegs;
	int mAlgorithm;
	int mIndicator;
	bool mSameColorAllowed ;
	bool mSetPins;
	bool mCloseRow;

	void createBoard();
	void createMenuBar();
	void DrawBoardBackground();

	QAction* toggleAllowSameColorAction;
	QAction* setPinsAutomaticallyAction;
	QAction* closeRowAutomaticallyAction;
	QAction* showIndicatorAction;

	QComboBox* pegsNumberComboBox;
	QComboBox* colorsNumberComboBox;
	QComboBox* solvingAlgorithmsComboBox;


};

#endif // MAINWINDOW_H
