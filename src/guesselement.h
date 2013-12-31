#ifndef GUESSELEMENT_H
#define GUESSELEMENT_H

#include "constants.h"

struct GuessElement
{
	void reset(const Algorithm &algorithm_m, const int &possibles_m);

	QString guess;
	QString code;
	int response;
	Algorithm algorithm;
	int possibles;
	qreal weight;
};

#endif // GUESSELEMENT_H
