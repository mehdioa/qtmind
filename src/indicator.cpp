#include "indicator.h"
#include <QSettings>

Indicator::Indicator()
{
	showColors = QSettings().value("ShowColors", 1).toBool();
	showIndicators =QSettings().value("ShowIndicators", 0).toBool();
	indicatorType = (IndicatorType) QSettings().value("IndicatorType", 0).toInt();
}
//-----------------------------------------------------------------------------

Indicator::~Indicator()
{
	QSettings().setValue("ShowColors", (int) showColors);
	QSettings().setValue("ShowIndicators", (int) showIndicators);
	QSettings().setValue("IndicatorType", (int) indicatorType);
}
