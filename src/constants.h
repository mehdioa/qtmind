#ifndef CONSTANTS_H
#define CONSTANTS_H

enum GAME_MODE {
	MODE_MASTER		= 0,//HUMAN BREAKS THE CODE
	MODE_BREAKER	= 1	//MACHINE BREAKS THE CODE
};

enum GAME_STATE {
	STATE_NONE		= 0,
	STATE_RUNNING	= 1,
	STATE_PAUSED	= 2,
	STATE_WON		= 3,
	STATE_LOST		= 4,
	STATE_GAVE_IN	= 5,
	STATE_ERROR		= 6
};


enum Algorithm {
	MostParts		= 0,
	WorstCase		= 1,
	ExpectedSize	=2
};


const int MIN_COLOR_NUMBER	= 2;
const int MAX_COLOR_NUMBER	= 10;

const int MIN_SLOT_NUMBER	= 2;
const int MAX_SLOT_NUMBER	= 5;

const int MAX_ROW_NUMBER	= 10;



#endif // CONSTANTS_H
