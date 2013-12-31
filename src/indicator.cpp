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
//-----------------------------------------------------------------------------

bool Indicator::forceColor() const
{
	return showColors || !showIndicators;
}
//-----------------------------------------------------------------------------

void Indicator::setShowColors(const bool &show_colors)
{
	showColors = show_colors;
}
//-----------------------------------------------------------------------------

void Indicator::setShowIndicators(const bool &show_indicators)
{
	showIndicators = show_indicators;
}
//-----------------------------------------------------------------------------

void Indicator::setIndicatorType(const IndicatorType &indicator_type)
{
	indicatorType = indicator_type;
}
//-----------------------------------------------------------------------------

bool Indicator::getShowColors() const
{
	return showColors;
}
//-----------------------------------------------------------------------------

bool Indicator::getShowIndicators() const
{
	return showIndicators;
}
//-----------------------------------------------------------------------------

IndicatorType Indicator::getIndicatorType() const
{
	return indicatorType;
}
