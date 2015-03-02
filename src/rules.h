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

#ifndef RULES_H
#define RULES_H

#include "appinfo.h"

/**
 * @brief The Rules class represents the rules of the game
 */
class Rules
{

public:
    /**
     * @brief instance singleton instance of the rules
     * @return the only instance of the rules
     */
    static Rules &instance();
    /**
     * @brief readSettings reads the application settings from the storage
     */
    void readSettings();
    /**
     * @brief writeSettings writes the application settings to the storage
     */
    void writeSettings();

private:
	Rules();
    ~Rules();
    Rules(const Rules &); // hide copy constructor
    Rules& operator=(const Rules &); // hide assign op

public:
    int mPegs; /**< TODO */
    int mColors; /**< TODO */
    bool mSameColors; /**< TODO */
    Algorithm mAlgorithm; /**< TODO */
    Mode mMode; /**< TODO */
};

#endif // RULES_H
