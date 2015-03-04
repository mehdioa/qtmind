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
#include "sounds.h"
#include <QSettings>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtMultimedia/QSoundEffect>
#endif

Sounds::Sounds(QObject *parent): QObject(parent)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    mPegDrop = new QSoundEffect;
    mPegDropRefuse = new QSoundEffect;
    mButtonPress = new QSoundEffect;
    mPegDrop->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegdrop.wav"));
    mPegDropRefuse->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegrefuse.wav"));
    mButtonPress->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pin.wav"));
#endif
    setVolume(QSettings().value("Volume", 3).toInt());
}

Sounds::~Sounds()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (mPegDrop)
        delete mPegDrop;
    if (mPegDropRefuse)
        delete mPegDropRefuse;
    if (mButtonPress)
        delete mButtonPress;
#endif
    QSettings().setValue("Volume", (int)mVolume);
}

void Sounds::setVolume(const int &vol)
{
    mVolume = static_cast<Volume>(vol);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    qreal real_volume = static_cast<qreal>(mVolume)/3;

    mPegDrop->setVolume(real_volume);
    mPegDropRefuse->setVolume(real_volume);
    mButtonPress->setVolume(real_volume);
#endif
}

Volume Sounds::volume() const
{
    return mVolume;
}

void Sounds::onPegDroped()
{
    if (mPegDrop)
        mPegDrop->play();
}

void Sounds::onPegDropRefused()
{
    if (mPegDropRefuse)
        mPegDropRefuse->play();
}

void Sounds::onButtonPressed()
{
    if (mButtonPress)
        mButtonPress->play();
}

