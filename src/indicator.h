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
	bool forceColor() const;
	void setShowColors(const bool &show_colors);
	void setShowIndicators(const bool &show_indicators);
	void setIndicatorType(const IndicatorType &indicator_type);
	bool getShowColors() const;
	bool getShowIndicators() const;
	IndicatorType getIndicatorType() const;

private:
	bool showColors;
	bool showIndicators;
	IndicatorType indicatorType;
};

#endif // INDICATOR_H
