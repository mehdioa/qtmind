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
#include "appinfo.h"
#include "rules.h"
#include "sounds.h"
#include "game.h"
#include "tools.h"
#include <QLocale>

class QComboBox;
class Sounds;

namespace Ui {
class MainWindow;
}


/**
 * @brief The MainWindow class is the main window that interacts with the user.
 * It is the bridge between the user and the game object. It pass every event to
 * the game object and decide what to do based on what the game replies.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event);

protected slots:
    void onAutoPutPins();
    void onAutoCloseRows();
    void onIndicatorChanged();
    void onModeChanged(QAction*);
    void onNewGame();
    void onFont();
    void onQtMindHomePage();
    void onAbout();
    void onShowContextMenu(const QPoint& position);
    void onAlgorithmChanded();
    void onVolumeChanged(QAction* volume_action);
    void onColorActionChanged(QAction* color_action);
    void onColorComboChanged(const int& combo_index);
    void onSlotActionChanged(QAction* slot_action);
    void onSlotComboChanged(const int& combo_index);
    void onAlgorithmActionChanged(QAction* algorithm_action);
    void onAlgorithmComboChanged(const int& combo_index);
    void onIndicatorTypeChanged(QAction* indic_act);
    void onLanguageChanged(QAction* language_act);

private:
    QStringList findTranslations();
    void loadTranslation();
    QString languageName(const QString& language);
    bool quitUnfinishedGame();
    void updateRules();
    void retranslate();
    void setPegsNumber(const int& pegs_n);
    Mode getMode();

private:
    static const bool sIsAndroid; /**< Is the platform Android? */
    Ui::MainWindow* ui; /**< TODO */
    QComboBox* mPegsComboBox; /**< TODO */
    QComboBox* mColorsComboBox; /**< TODO */
    QComboBox* mAlgorithmsComboBox; /**< TODO */

    Game mGame; /**< TODO */
    Sounds mSounds;
    QString mAppPath; /**< TODO */
//    Rules mRules;
    Tools mTools;
};

#endif // MAINWINDOW_H
