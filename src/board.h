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
		Character,
		Digit
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

	/**
	 * @brief Creates an object by reading settings.
	 */
	Board();
	~Board();

	QString getFontName() const { return fontName; }
	int getFontSize() const { return fontSize; }
	bool isAutoPutPins() const { return autoPutPins; }
	bool isAutoCloseRows() const { return autoCloseRows; }
	Indicator getIndicator() const { return indicator; }
	bool getShowIndicators() const { return showIndicators;}
	QLocale *getLoale() {return &m_locale; }
	bool getShowColors() const { return showColors; }
	bool forceColor() const;
	void play(Sound s) const;
	void setVolume(const int &vol);
	Volume getVolume() const {return volume;}
	QLocale *locale() { return &m_locale; }
	void setLocale(const QString &name);
	void setAutoPutPins(const bool &a);
	void setAutoCloseRows(const bool &a);
	void setIndicators(const bool &show_indicators, const bool &show_colors);
	void setIndicator(int a);
	void setFont(const QString &name, const int &_size);

private:
	bool autoPutPins; /**< TODO */
	bool autoCloseRows; /**< TODO */
	bool showColors; /**< TODO */
	bool showIndicators; /**< TODO */
	Indicator indicator; /**< TODO */
	QLocale m_locale; /**< TODO */
	QString fontName; /**< TODO */
	int fontSize; /**< TODO */
	Volume volume; /**< TODO */
	QSoundEffect *pegDrop; /**< TODO */
	QSoundEffect *pegDropRefuse; /**< TODO */
	QSoundEffect *buttonPress; /**< TODO */
};

#endif // BOARD_H
