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

#include "preferences.h"
#include "ui_preferences.h"
#include "boardaid.h"

#include <QFile>
#include <QSettings>
#include <QLibraryInfo>
#include <QDir>
#include "constants.h"

QString Preferences::AppPath;

//-----------------------------------------------------------------------------

Preferences::Preferences(BoardAid *board_aid, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Preferences),
	boardAid(board_aid)
{
	ui->setupUi(this);
	setLayoutDirection(boardAid->locale.textDirection());

	for(int i = 10; i < 21; ++i) {
		ui->fontSizeComboBox->addItem(QString("%1 %2").arg(boardAid->locale.toString(i)).arg(tr("Point(s)", "", i)));
	}
	ui->fontComboBox->setCurrentFont(boardAid->boardFont.fontName);
	ui->fontSizeComboBox->setCurrentIndex(boardAid->boardFont.fontSize - 10);

	connect(ui->acceptRejectButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui->acceptRejectButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

}
//-----------------------------------------------------------------------------

Preferences::~Preferences()
{
	delete ui;
}
//-----------------------------------------------------------------------------

void Preferences::accept()
{
	QDialog::accept();
	boardAid->boardFont.fontName = ui->fontComboBox->currentText();
	boardAid->boardFont.fontSize = ui->fontSizeComboBox->currentIndex() + 10;
}
