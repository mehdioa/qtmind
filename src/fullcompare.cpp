#include "fullcompare.h"

FullCompare::FullCompare (const int &p, const int &c, const int &bG,
						  const int &mG, const Algorithm &alg):
	pegs(p), colors(c), lengthOfAllPossibles(1),
	beginigGuess(bG), maxGuess(mG), algorithm(alg)
{
	for(int i = 0; i < pegs; ++i) {
		lengthOfAllPossibles *= colors;
	}
	QList<unsigned short> currentCompare;
	for (int i = 0; i < lengthOfAllPossibles; ++i) {
		currentCompare.clear();
		for (int j = 0; j < lengthOfAllPossibles; ++j) {
			currentCompare.push_back(compare(i, j));
		}
		setOfAllCompares.push_back(currentCompare);
	}
	lengthOfFullGuess = (pegs+1)*(pegs+2)/2;
}

void FullCompare::indexToNumber(const int index, int *number) const
{
	/* Pre: This function get an index and a pointer to a int[pegs] array
	 * Post: Find the index-th consequent pegs-digit number.*/
	int tempint = index;
	for(int j=0; j < pegs; ++j) {
		*(number + pegs - 1 - j) = tempint%colors;
		tempint /= colors;
	}
}

short int FullCompare::compare (const int &indexA, const int &indexB) const
{
	/* Compare A to guess and see if its bw-code = response
	 * int c[colors] = {0}, g[colors] = {0}; for later */
	int c[colors], g[colors];
	for(int i = 0; i < colors; ++i) {
		c[i] = 0;
		g[i] =0;
	}
	int q1[pegs], q2[pegs];
	indexToNumber(indexA,q1);
	indexToNumber(indexB,q2);
	int b = 0, w = 0;
	for (int i = 0; i < pegs; ++i) {
		if (q1[i] == q2[i]) ++b;
		++c[q1[i]];
		++g[q2[i]];
	}
	for (int i = 0; i < colors; ++i){
		w += std::min(c[i],g[i]);
	}
	w -= b;
	return (b + w)*(b + w + 1)/2 + b;
}
