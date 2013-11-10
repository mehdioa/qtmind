#ifndef FULLCOMPARE_H
#define FULLCOMPARE_H

#include <QList>
#include <vector>
enum Algorithm {MostParts, WorstCase, ExpectedSize};

class FullCompare {
public:
	FullCompare(const int &p = 4, const int &c = 6,  const int &bGg = 8, const int &mG = 9,
				const Algorithm &alg = MostParts);
	void indexToNumber (const int index, int *number) const;
	short int compare (const int &indexA, const int &indexB) const;

protected:
	int pegs;
	int colors;
	unsigned short lengthOfFullGuess;
	int lengthOfAllPossibles;
	unsigned short beginigGuess;
	unsigned short maxGuess;
	Algorithm algorithm;
	QList<QList <short unsigned>>  setOfAllCompares;
	//Methods
};
#endif // FULLCOMPARE_H
