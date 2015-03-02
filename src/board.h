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
class QSettings;

/**
 * @brief A class to represent board tools, like font, locale, indicator.
 */
class Board
{
public:
    /**
     * @brief The Indicator enum
     */
	enum class Indicator {
		Digit = 48,     // begining of digits in unicode
		Character = 65, // begining of characters in unicode
	};
    /**
     * @brief The Sound enum
     */
	enum class Sound {
		PegDrop,
		PegDropRefuse,
		ButtonPress
	};
    /**
     * @brief The Volume enum
     */
	enum class Volume{
		Mute,
		Low,
		Medium,
		High
	};
    /**
     * @brief instance static instance of the Board singleton
     * @return the static singleton of the Board singleton
     */
    static Board &instance();

	/**
	 * @brief forceColor shows if colors of pegs should be visible
     * @return true if colors should be enabled, false otherwise
	 */
    bool forceColor() const;

	/**
	 * @brief play button push or peg drop sound
     * @param s the sound to be played
	 */
    void play(const Sound &s) const;

	/**
	 * @brief setVolume set the volume of the game
     * @param vol the new volume
	 */
    void setVolume(const int &vol);
    /**
     * @brief showIndicators idicate that if indicators should be enabled
     * @return true if indicators are enabled, false othewise
     */
    bool showIndicators() {return mShowIndicators; }
    /**
     * @brief getIndicatorIndex gives the indicator index as an integer
     * @return the index of the indicators
     */
    int getIndicatorIndex() {return (int)mIndicator; }
    /**
     * @brief getFontName get the font name of the game
     * @return the font name of the game
     */
    QString getFontName() {return mFontName; }
    /**
     * @brief getFontSize get the font size of the game
     * @return the font size
     */
    int getFontSize() {return mFontSize; }
    /**
     * @brief getLocale get the locale of the game
     * @return the locale of the game
     */
    QLocale &getLocale() { return mLocale; }
    /**
     * @brief isAutoCloseRows shows that if auto close rows is enabled
     * @return true if auto close rows is enabled, false otherwise
     */
    bool isAutoCloseRows() { return mAutoCloseRows; }
    /**
     * @brief isAutoPutPins shows that if auto put pins is enabled
     * @return true if auto put pins is enabled, false othewise
     */
    bool isAutoPutPins() { return mAutoPutPins; }
    /**
     * @brief readSettings read the settings from the application settings
     */
    void readSettings();
    /**
     * @brief writeSettings write the application settings to the storage
     */
    void writeSettings();

private:
    /**
     * @brief Creates an object by reading settings->
     */
    Board();
    ~Board();	// hide destructor
    Board(const Board &); // hide copy constructor
    Board& operator=(const Board &); // hide assign op


private:
    QLocale mLocale; /**< TODO */
    bool mAutoPutPins; /**< TODO */
    bool mAutoCloseRows; /**< TODO */
    bool mShowColors; /**< TODO */
    bool mShowIndicators; /**< TODO */
    Indicator mIndicator; /**< TODO */
    QString mFontName; /**< TODO */
    int mFontSize; /**< TODO */
    Volume mVolume; /**< TODO */

    QSoundEffect *mPegDrop; /**< TODO */
    QSoundEffect *mPegDropRefuse; /**< TODO */
    QSoundEffect *mButtonPress; /**< TODO */

    friend class MainWindow;
    friend class Preferences;
};

#endif // BOARD_H
