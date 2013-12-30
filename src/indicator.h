#ifndef INDICATOR_H
#define INDICATOR_H

enum class IndicatorType
{
	Character,
	Digit
};


class Indicator
{
public:
	Indicator();
	~Indicator();
	bool forceColor() const {return showColors || !showIndicators;}

//private:
	bool showColors;
	bool showIndicators;
	IndicatorType indicatorType;
};

#endif // INDICATOR_H
