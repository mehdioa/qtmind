#ifndef GAME_H
#define GAME_H
#include <QList>
#include <QStringList>
#include <QVector>
#include "constants.h"

class Game
{
	/*	The class Game is the engine of the mastermid game. It contains all the solving
	 *	algorithms and auxiliary functions that provide efficient code guess and handling
	 *	response and so on
	 */
public:
	Game(const int& peg_no = 4, const int& color_no = 6, const bool& allow_same_color = true);
	~Game();
//	QString indexToCode(const int &index) const;
	int compare (const QString codeA, const QString codeB) const;
	bool done () const;
	QString getGuess();
	QString getGuess(const Algorithm& alg);
	bool setResponse(const int& response);
	bool hasDifferentSetup(const int& peg_no, const int color_no) const;
	void reset(const int& peg_no = 4, const int& color_no = 6, const bool& allow_same_color = true);

private:
	QString mostPartsAlgorithm() const;					//	most parts solving algorithm
	QString expectedSizeAlgorithm () const;					//	expected size solving algorithm
	QString worstCaseAlgorithm () const;					//	worst case solving algorithm
	QString convertBase(int decimal, const int &base, const int &precision);
//	QString convert(int decimal, int base, int precision);
	void setAllCodesSize();

	void makeGuess(Algorithm alg);						//	create a guess, based on the game algorithm and put it in mGuess
	void fillAllCodes();
	void clearAllCodes();


	int mPegNumber;										//	pegs count, 4
	int mColorNumber;									//	colors count, 6
	bool mAllowSameColor;
	QString mGuess;											//	the guess number, ?
	int mResponse;										//	the black-white response code, [0..14]
	int mResponeSpaceSize;
	int mAllCodesSize;									//	the size of the complete code space, 6^4 = 1296

	QString* mAllCodes;									//	0..1295
	QVector<int> mPossibleCodes;							//	list of all possibles

};

#endif // GAME_H
