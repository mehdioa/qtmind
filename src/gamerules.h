#ifndef GAMERULES_H
#define GAMERULES_H

#include "constants.h"
#include <QLocale>

enum class GameMode {
	Codemaker,		//HUMAN MAKE THE HIDDEN CODE
	Codebreaker		//HUMAN BREAKS THE HIDDEN CODE
};

struct GameRules
{
	GameRules();
	~GameRules();

	int pegNumber;
	int colorNumber;
	bool sameColorAllowed;
	Algorithm algorithm;
	GameMode gameMode;
};

#endif // GAMERULES_H
