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
#include "tools.h"

#include <QFile>
#include <QLibraryInfo>
#include <QDir>

Preferences::Preferences(Tools* tools, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Preferences),
    mTools(tools)
{
    ui->setupUi(this);
    setLayoutDirection(mTools->mLocale.textDirection());

    for(int i = 10; i < 21; ++i) {
        ui->sizeComboBox->addItem(QString("%1 %2").arg(mTools->mLocale.toString(i)).arg(tr("Point(s)", "", i)));
    }
    ui->fontComboBox->setCurrentFont(mTools->mFontName);
    ui->sizeComboBox->setCurrentIndex(mTools->mFontSize - 10);

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
    QString fontName = ui->fontComboBox->currentText();
    int fontSize = ui->sizeComboBox->currentIndex() + 10;
    if (mTools->mFontName != fontName ||
            mTools->mFontSize != fontSize) {
        mTools->mFontName = fontName;
        mTools->mFontSize = fontSize;
        emit fontChangedSignal();
    }
}
