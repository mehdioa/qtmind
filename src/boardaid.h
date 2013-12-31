#ifndef BOARDAID_H
#define BOARDAID_H

#include "indicator.h"
#include "boardsounds.h"
#include "boardfont.h"
#include <QString>
#include <QLocale>
#include <QFont>



struct BoardAid
{
	BoardAid();
	~BoardAid();

	bool autoPutPins;
	bool autoCloseRows;
	Indicator indicator;
	QLocale locale;
	BoardFont boardFont;
	BoardSounds boardSounds;
};

#endif // BOARDAID_H
