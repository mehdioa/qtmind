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
#else
#include <QUrl>
struct QSoundEffect {
    void play(){}
    void setSource(const QUrl &){}
    void setVolume(const qreal &) {}
	void deleteLater() {qDebug("QSoundEffect Cleared");}
};
#endif

Sounds::Sounds(QObject* parent): QObject(parent)
{
    mPegDrop =  QSharedPointer<QSoundEffect>(new QSoundEffect, &QSoundEffect::deleteLater);
    mPegDropRefuse =  QSharedPointer<QSoundEffect>(new QSoundEffect, &QSoundEffect::deleteLater);
    mButtonPress =  QSharedPointer<QSoundEffect>(new QSoundEffect, &QSoundEffect::deleteLater);
    mPegDrop.data()->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegdrop.wav"));
    mPegDropRefuse.data()->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegrefuse.wav"));
    mButtonPress.data()->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pin.wav"));
    setVolume(QSettings().value("Volume", 3).toInt());
}

Sounds::~Sounds()
{
    QSettings().setValue("Volume", (int)mVolume);
}

void Sounds::setVolume(const int& vol)
{
    mVolume = static_cast<Volume>(vol);
    qreal real_volume = static_cast<qreal>(mVolume)/3;
    mPegDrop.data()->setVolume(real_volume);
    mPegDropRefuse.data()->setVolume(real_volume);
    mButtonPress.data()->setVolume(real_volume);
}

Volume Sounds::volume() const
{
	return mVolume;
}

void Sounds::onPegDroped()
{
    mPegDrop.data()->play();
}

void Sounds::onPegDropRefused()
{
    mPegDropRefuse.data()->play();
}

void Sounds::onButtonPressed()
{
    mButtonPress.data()->play();
}

