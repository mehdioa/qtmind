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
#include "QSettings"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtMultimedia/QSoundEffect>
#endif

Board::Board():
	m_locale(QSettings().value("Locale/Language", "en").toString().left(5)),
	m_autoPutPins(QSettings().value("AutoPutPins", true).toBool()),
	m_autoCloseRows(QSettings().value("AutoCloseRows", false).toBool()),
	m_showColors(QSettings().value("ShowColors", 1).toBool()),
	m_showIndicators(QSettings().value("ShowIndicators", 0).toBool()),
	m_indicator((Indicator) QSettings().value("Indicator", 0).toInt()),
	m_fontName(QSettings().value("FontName", "SansSerif").toString()),
	m_fontSize((QSettings().value("FontSize", 12).toInt()))
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	m_pegDrop = new QSoundEffect;
	m_pegDropRefuse = new QSoundEffect;
	m_buttonPress = new QSoundEffect;
	m_pegDrop->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegdrop.wav"));
	m_pegDropRefuse->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegrefuse.wav"));
	m_buttonPress->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pin.wav"));
#endif
	setVolume(QSettings().value("Volume", 3).toInt());
}

Board *Board::instance()
{
	static Board board;
	return &board;
}

Board::~Board()
{
	QSettings().setValue("AutoPutPins",	m_autoPutPins);
	QSettings().setValue("AutoCloseRows", m_autoCloseRows);
	QSettings().setValue("FontName", m_fontName);
	QSettings().setValue("FontSize", m_fontSize);
	QSettings().setValue("ShowColors", (int) m_showColors);
	QSettings().setValue("ShowIndicators", (int) m_showIndicators);
	QSettings().setValue("Indicator", (int) m_indicator);
	QSettings().setValue("Locale/Language", m_locale.name());
	QSettings().setValue("Volume", static_cast<int>(m_volume));
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	delete m_pegDrop;
	delete m_pegDropRefuse;
	delete m_buttonPress;
#endif
}

bool Board::forceColor() const
{
	return m_showColors || !m_showIndicators;
}

void Board::play(Sound s) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	switch (s) {
	case Sound::PegDrop:
		m_pegDrop->play();
		break;
	case Sound::PegDropRefuse:
		m_pegDropRefuse->play();
		break;
	default:
		m_buttonPress->play();
		break;
	}
#endif
}

void Board::setVolume(const int &vol)
{
	m_volume = static_cast<Volume>(vol);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	qreal real_volume = static_cast<qreal>(m_volume)/3;

	m_pegDrop->setVolume(real_volume);
	m_pegDropRefuse->setVolume(real_volume);
	m_buttonPress->setVolume(real_volume);
#endif
}
