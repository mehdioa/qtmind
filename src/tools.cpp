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
#include "tools.h"
#include <QSettings>

Tools::Tools()
{
    QSettings settings;
    mFontName = settings.value("FontName", "SansSerif").toString();
    mFontSize = settings.value("FontSize", 12).toInt();
    mAutoPutPins = settings.value("AutoPutPins", true).toBool();
    mAutoCloseRows = settings.value("AutoCloseRows", false).toBool();
    mLocale = QLocale(QSettings().value("Locale/Language", "en").toString().left(5));
    mLocale.setNumberOptions(QLocale::OmitGroupSeparator);
}

Tools::~Tools()
{
    QSettings settings;
    settings.setValue("FontName", mFontName);
    settings.setValue("FontSize", mFontSize);
    settings.setValue("AutoPutPins",    mAutoPutPins);
    settings.setValue("AutoCloseRows", mAutoCloseRows);
    QSettings().setValue("Locale/Language", mLocale.name());
}

