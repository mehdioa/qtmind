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

#ifndef BOARD_H
#define BOARD_H

#include <QLocale>

class QSoundEffect;

/**
 * @brief A class to represent board tools, like font, locale, indicator.
 */
class Board
{
public:
	enum class Indicator {
		Digit = 48,     // begining of digits in unicode
		Character = 65, // begining of characters in unicode
	};
	enum class Sound {
		PegDrop,
		PegDropRefuse,
		ButtonPress
	};
	enum class Volume{
		Mute,
		Low,
		Medium,
		High
	};

	static Board *instance();
	~Board();

	/**
	 * @brief forceColor shows if colors of pegs should be visible
	 * @return
	 */
	bool forceColor() const;

	/**
	 * @brief play button push or peg drop sound
	 * @param s
	 */
	void play(Sound s) const;

	/**
	 * @brief setVolume set the volume of the game
	 * @param vol
	 */
	void setVolume(const int &vol);

private:
	/**
	 * @brief Creates an object by reading settings.
	 */
	Board();

public:
	QLocale m_locale; /**< TODO */
	bool m_autoPutPins; /**< TODO */
	bool m_autoCloseRows; /**< TODO */
	bool m_showColors; /**< TODO */
	bool m_showIndicators; /**< TODO */
	Indicator m_indicator; /**< TODO */
	QString m_fontName; /**< TODO */
	int m_fontSize; /**< TODO */
	Volume m_volume; /**< TODO */

private:
	QSoundEffect *m_pegDrop; /**< TODO */
	QSoundEffect *m_pegDropRefuse; /**< TODO */
	QSoundEffect *m_buttonPress; /**< TODO */
};

#endif // BOARD_H
