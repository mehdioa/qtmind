#include "gamerules.h"
#include <QSettings>

GameRules::GameRules():
	pegNumber(QSettings().value("Pegs", 4).toInt()),
	colorNumber(QSettings().value("Colors", 6).toInt()),
	sameColorAllowed(QSettings().value("SameColor", true).toBool()),
	algorithm((Algorithm) QSettings().value("Algorithm", 0).toInt()),
	gameMode((GameMode) QSettings().value("Mode", 0).toInt())
{

}
//-----------------------------------------------------------------------------

GameRules::~GameRules()
{
	QSettings().setValue("Mode", (int) gameMode);
	QSettings().setValue("Colors", colorNumber);
	QSettings().setValue("Pegs", pegNumber);
	QSettings().setValue("Algorithm", (int) algorithm);
	QSettings().setValue("SameColor", sameColorAllowed);
}
