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

#include "board.h"
#include "appinfo.h"
#include "QSettings"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtMultimedia/QSoundEffect>
#endif

Board::Board():
	autoPutPins(QSettings().value("AutoPutPins", true).toBool()),
	autoCloseRows(QSettings().value("AutoCloseRows", false).toBool()),
	showColors(QSettings().value("ShowColors", 1).toBool()),
	showIndicators(QSettings().value("ShowIndicators", 0).toBool()),
	indicator((Indicator) QSettings().value("Indicator", 0).toInt()),
	m_locale(QSettings().value("Locale/Language", "en").toString().left(5)),
	fontName(QSettings().value("FontName", "SansSerif").toString()),
	fontSize((QSettings().value("FontSize", 12).toInt()))
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	pegDrop = new QSoundEffect;
	pegDropRefuse = new QSoundEffect;
	buttonPress = new QSoundEffect;
	pegDrop->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegdrop.wav"));
	pegDropRefuse->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegrefuse.wav"));
	buttonPress->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pin.wav"));
#endif
	setVolume(QSettings().value("Volume", 3).toInt());
}

Board::~Board()
{
	QSettings().setValue("AutoPutPins",	autoPutPins);
	QSettings().setValue("AutoCloseRows", autoCloseRows);
	QSettings().setValue("FontName", fontName);
	QSettings().setValue("FontSize", fontSize);
	QSettings().setValue("ShowColors", (int) showColors);
	QSettings().setValue("ShowIndicators", (int) showIndicators);
	QSettings().setValue("Indicator", (int) indicator);
	QSettings().setValue("Locale/Language", m_locale.name());
	QSettings().setValue("Volume", static_cast<int>(volume));
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	delete pegDrop;
	delete pegDropRefuse;
	delete buttonPress;
#endif
}

bool Board::forceColor() const
{
	return showColors || !showIndicators;
}

void Board::play(Sound s) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	switch (s) {
	case Sound::PegDrop:
		pegDrop->play();
		break;
	case Sound::PegDropRefuse:
		pegDropRefuse->play();
		break;
	default:
		buttonPress->play();
		break;
	}
#endif
}

void Board::setVolume(const int &vol)
{
	volume = static_cast<Volume>(vol);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	qreal real_volume = static_cast<qreal>(volume)/3;

	pegDrop->setVolume(real_volume);
	pegDropRefuse->setVolume(real_volume);
	buttonPress->setVolume(real_volume);
#endif
}

void Board::setLocale(const QString &name)
{
	QLocale newLocale(name);
	m_locale = newLocale;

}

void Board::setAutoPutPins(const bool &a)
{
	autoPutPins = a;
}

void Board::setAutoCloseRows(const bool &a)
{
	autoCloseRows = a;
}

void Board::setIndicators(const bool &show_indicators, const bool &show_colors)
{
	showIndicators = show_indicators;
	showColors = show_colors;
}

void Board::setIndicator(int a)
{
	indicator = static_cast<Indicator>(a);
}

void Board::setFont(const QString &name, const int &_size)
{
	fontName = name;
	fontSize = _size;
}
