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
#ifndef SOUNDS_H
#define SOUNDS_H

#include "appinfo.h"
#include <QObject>
#include <QSharedPointer>

class QSoundEffect;

class Sounds: public QObject
{
    Q_OBJECT
public:
    explicit Sounds(QObject* parent = 0);
    ~Sounds();

    void setVolume(const int& vol);
	Volume volume() const;

public slots:
    void onPegDroped();
    void onPegDropRefused();
    void onButtonPressed();

private:
    Volume mVolume; /**< TODO */
    QSharedPointer<QSoundEffect> mPegDrop; /**< TODO */
    QSharedPointer<QSoundEffect> mPegDropRefuse; /**< TODO */
    QSharedPointer<QSoundEffect> mButtonPress; /**< TODO */
};

#endif // SOUNDS_H
