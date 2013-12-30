#include "boardaid.h"
#include "QSettings"

BoardAid::BoardAid():
	autoPutPins(QSettings().value("SetPins", true).toBool()),
	autoCloseRows(QSettings().value("AutoCloseRows", true).toBool()),
	locale(QSettings().value("Locale/Language", "en").toString().left(5))
{
}

BoardAid::~BoardAid()
{
	QSettings().setValue("SetPins",	autoPutPins);
	QSettings().setValue("AutoCloseRows", autoCloseRows);
	QSettings().setValue("Locale/Language", locale.name());
}
