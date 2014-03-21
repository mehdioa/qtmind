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

#ifndef BOARDSOUNDS_H
#define BOARDSOUNDS_H

class QSoundEffect;

class BoardSounds
{
public:
	enum Volume{
		Mute,
		Low,
		Medium,
		High
	};

	BoardSounds();
	~BoardSounds();
	void playPegDropSound();
	void playPegDropRefuseSound();
	void playButtonPressSound();
	void setVolume(const int &vol);
	Volume getVolume() const {return volume;}

private:
	Volume volume;

	QSoundEffect *pegDropSound;
	QSoundEffect *pegDropRefuseSound;
	QSoundEffect *buttonPressSound;
};

#endif // BOARDSOUNDS_H
