#include "guesselement.h"

void GuessElement::reset(const Algorithm &algorithm_m, const int &possibles_m)
{
	guess = "";
	code = "";
	response = 0;
	algorithm = algorithm_m;
	possibles = possibles_m;
	weight = 0;
}
