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

/**
 * @brief A class to define board sounds. It needs Qt at least 5.0.0 to
 * work.
 */
class Sounds
{
public:
	enum Volume{
		Mute,
		Low,
		Medium,
		High
	};

	/**
	 * @brief Sounds
	 */
	Sounds();
	~Sounds();
	/**
	 * @brief playPegDrop
	 */
	void playPegDrop();
	/**
	 * @brief playPegDropRefuse
	 */
	void playPegDropRefuse();
	/**
	 * @brief playButtonPress
	 */
	void playButtonPress();
	/**
	 * @brief setVolume
	 * @param vol
	 */
	void setVolume(const int &vol);
	/**
	 * @brief getVolume
	 * @return
	 */
	Volume getVolume() const {return volume;}

private:
	Volume volume; /**< TODO */

	QSoundEffect *pegDrop; /**< TODO */
	QSoundEffect *pegDropRefuse; /**< TODO */
	QSoundEffect *buttonPress; /**< TODO */
};

#endif // BOARDSOUNDS_H
