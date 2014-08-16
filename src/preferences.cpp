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
#include "board.h"

#include <QFile>
#include <QSettings>
#include <QLibraryInfo>
#include <QDir>

QString Preferences::AppPath; /**< TODO */

Preferences::Preferences(Board *board_aid, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Preferences),
	board(board_aid)
{
	ui->setupUi(this);
	setLayoutDirection(board->locale()->textDirection());

	for(int i = 10; i < 21; ++i) {
		ui->sizeComboBox->addItem(QString("%1 %2").arg(board->locale()->toString(i)).arg(tr("Point(s)", "", i)));
	}
	ui->fontComboBox->setCurrentFont(board->getFontName());
	ui->sizeComboBox->setCurrentIndex(board->getFontSize() - 10);

	connect(ui->acceptRejectButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui->acceptRejectButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

}

Preferences::~Preferences()
{
	delete ui;
}

void Preferences::accept()
{
	QDialog::accept();
	board->setFont(ui->fontComboBox->currentText(),
				   ui->sizeComboBox->currentIndex() + 10);
}
