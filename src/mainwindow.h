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
#include "board.h"

class QMenu;
class QAction;
class QToolBar;
class QActionGroup;
class QComboBox;
class Board;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void closeEvent(QCloseEvent *event); /** Override parent function to save window geometry. */

private slots:
	void onSetPinsCloseRowAutomatically();
	void onIndicatorChanged();
	void onGameModeChanged(QAction*);
	void onUpdateNumbers();
	void onNewGame();
	void onPreferences();
	void onAbout();
	void onShowContextMenu(const QPoint& position);

signals:
	void showIndicatorsSignal(bool show_colors, bool show_indicators, IndicatorType);
	void preferencesChangeSignal();

private:
	void setPegsComboBox();
	void setColorsComboBox();
	void setSolvingAlgorithmsComboBox();

private:
	Ui::MainWindow *ui;

	Board *mBoard;
	GameMode mGameMode;
	int mColorNumber;
	int mPegNumber;
	Algorithm mAlgorithm;
	bool mShowColors;
	bool mShowIndicators;
	IndicatorType mIndicatorType;
	bool mSameColorAllowed;
	bool mAutoPutPins;
	bool mAutoCloseRows;
	QLocale mLocale;
	int mVolume;

	QComboBox *pegsNumberComboBox;
	QComboBox *colorsNumberComboBox;
	QComboBox *solvingAlgorithmsComboBox;
	QAction *allowSameColorAction;
};

#endif // MAINWINDOW_H
