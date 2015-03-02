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
#include <QSettings>
#include <QMutex>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtMultimedia/QSoundEffect>
#endif

Board* Board::sBoard = 0;

Board::Board()
{
    QSettings settings;
    mLocale = settings.value("Locale/Language", "en").toString().left(5);
    mAutoPutPins = settings.value("AutoPutPins", true).toBool();
    mAutoCloseRows = settings.value("AutoCloseRows", false).toBool();
    mShowColors = settings.value("ShowColors", 1).toBool();
    mShowIndicators = settings.value("ShowIndicators", 0).toBool();
    mIndicator = (Indicator) settings.value("Indicator", 65).toInt();
    mFontName = settings.value("FontName", "SansSerif").toString();
    mFontSize = settings.value("FontSize", 12).toInt();
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    mPegDrop = new QSoundEffect;
    mPegDropRefuse = new QSoundEffect;
    mButtonPress = new QSoundEffect;
    mPegDrop->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegdrop.wav"));
    mPegDropRefuse->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegrefuse.wav"));
    mButtonPress->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pin.wav"));
#endif
    setVolume(settings.value("Volume", 3).toInt());
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    mPegDrop = new QSoundEffect;
    mPegDropRefuse = new QSoundEffect;
    mButtonPress = new QSoundEffect;
    mPegDrop->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegdrop.wav"));
    mPegDropRefuse->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegrefuse.wav"));
    mButtonPress->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pin.wav"));
#endif
}

Board *Board::instance()
{
    static QMutex mutex;
    if (!sBoard) {
        mutex.lock();
        if (!sBoard) {
            sBoard = new Board;
        }
        mutex.unlock();
    }
    return sBoard;
}

Board::~Board()
{
    QSettings settings;
    settings.setValue("AutoPutPins",	mAutoPutPins);
    settings.setValue("AutoCloseRows", mAutoCloseRows);
    settings.setValue("FontName", mFontName);
    settings.setValue("FontSize", mFontSize);
    settings.setValue("ShowColors", (int) mShowColors);
    settings.setValue("ShowIndicators", (int) mShowIndicators);
    settings.setValue("Indicator", (int) mIndicator);
    settings.setValue("Locale/Language", mLocale.name());
    settings.setValue("Volume", static_cast<int>(mVolume));
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (mPegDrop)
        delete mPegDrop;
    if (mPegDropRefuse)
        delete mPegDropRefuse;
    if (mButtonPress)
        delete mButtonPress;
#endif
}
Indicator Board::indicator() const
{
    return mIndicator;
}

//void Board::setIndicator(const Indicator &indicator)
//{
//    mIndicator = indicator;
//}

bool Board::hasShowColors() const
{
    return mShowColors;
}

//void Board::setShowColors(bool showColors)
//{
//    mShowColors = showColors;
//}


bool Board::hasForceColor() const
{
    return mShowColors || !mShowIndicators;
}

void Board::play(const Sound &s) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	switch (s) {
	case Sound::PegDrop:
        mPegDrop->play();
		break;
	case Sound::PegDropRefuse:
        mPegDropRefuse->play();
		break;
	default:
        mButtonPress->play();
		break;
	}
#endif
}

void Board::setVolume(const int &vol)
{
    mVolume = static_cast<Volume>(vol);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    qreal real_volume = static_cast<qreal>(mVolume)/3;

    mPegDrop->setVolume(real_volume);
    mPegDropRefuse->setVolume(real_volume);
    mButtonPress->setVolume(real_volume);
#endif
}

Volume Board::volume() const
{
    return mVolume;
}
