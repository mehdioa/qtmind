#ifndef BOARD_H
#define BOARD_H
#include "constants.h"
#include <QGraphicsView>

class Peg;
class EmptyBox;
class PinBox;
class PegBox;
class Button;
class Game;
class Message;

class Board: public QGraphicsView
{
	Q_OBJECT

public:
	Board(QWidget* parent = 0);
	~Board();
	void handleButtonRelease(const QString &);
	void generate();
	void play(const int &mode);
	void setPinsRow(const bool &set_pins, const bool &closeRow);
	void reset(const int& peg_n, const int& color_n, const int &mode_n,
			   const bool &samecolor, const int &algorithm, const bool &set_pins,
			   const bool &close_row);

protected:
	void drawBackground(QPainter* painter, const QRectF& rect);
	void drawForeground(QPainter* painter, const QRectF& rect);
	void resizeEvent(QResizeEvent* event);

signals:
	void pegIndicatorSignal(bool enabled);

private slots:
	void onPegMouseRelease(QPoint position, int color);
	void onOkButtonPressed();
	void onDoneButtonPressed();
	void onPinBoxPressed();

private:
	void playCodeMaster();
	void playCodeBreaker();
	void createBoxes();
//	void getCode();
	void createPegForBox(PegBox* box, int color, bool backPeg = false);// backPeg should only be used to put an extra peg under initial pegs,
	void codeRowFilled(bool filled);


	QList<PinBox*> mPinBoxes;
	QList<PegBox*> mPegBoxes;
	QList<PegBox*> mCodeBoxes;
	QList<PegBox*> mCurrentBoxes;	//	boxes that can be filled by player
	QList<PegBox*> mMasterBoxes;

	GAME_STATE mState;
	GAME_MODE mMode;
	int mPegNumber;
	int mColorNumber;
	Algorithm mAlgorithm;
	bool mSameColor;
	bool mSetPins;
	bool mCloseRow;
	bool mDone;
	Game* mGame;
	Button* mOkButton;
	Button* mDoneButton;
	Message* mMessage;
	QString mMasterCode;
	QString mGuess;

//	bool rowFillState;
};

#endif // BOARD_H
