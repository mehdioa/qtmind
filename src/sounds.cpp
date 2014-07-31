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
#include "QSettings"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtMultimedia/QSoundEffect>
#endif

Sounds::Sounds()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	pegDrop = new QSoundEffect;
	pegDropRefuse = new QSoundEffect;
	buttonPress = new QSoundEffect;
	pegDrop->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegdrop.wav"));
	pegDropRefuse->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pegrefuse.wav"));
	buttonPress->setSource(QUrl::fromLocalFile("://sounds/resources/sounds/pin.wav"));
#endif
	setVolume(static_cast<Volume>(QSettings().value("Volume", 3).toInt()));
}

Sounds::~Sounds()
{
	QSettings().setValue("Volume", static_cast<int>(volume));
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	delete pegDrop;
	delete pegDropRefuse;
	delete buttonPress;
#endif
}

void Sounds::playPegDrop()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	pegDrop->play();
#endif
}

void Sounds::playPegDropRefuse()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	pegDropRefuse->play();
#endif
}

void Sounds::playButtonPress()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	buttonPress->play();
#endif
}

void Sounds::setVolume(const int &vol)
{
	volume = static_cast<Volume>(vol);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	qreal real_volume = static_cast<qreal>(volume)/3;

	pegDrop->setVolume(real_volume);
	pegDropRefuse->setVolume(real_volume);
	buttonPress->setVolume(real_volume);
#endif
}
