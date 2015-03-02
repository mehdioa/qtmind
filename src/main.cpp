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

#include "mainwindow.h"
#include "appinfo.h"
#include "board.h"
#include "rules.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VER);
    app.setOrganizationName(ORG_NAME);
    app.setOrganizationDomain(ORG_DOMAIN);

//    QSettings settings;
    Board::instance().readSettings();
    Rules::instance().readSettings();

    MainWindow w; // MainWindow will delete game
    w.setWindowIcon(QIcon("://icons/resources/icons/qtmind.png"));
    w.show();
    int i = app.exec();
    Board::instance().writeSettings();
    Rules::instance().writeSettings();
    return i;
}
