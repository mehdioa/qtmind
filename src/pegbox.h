#ifndef PEGBOX_H
#define PEGBOX_H
#include "emptybox.h"

enum PEG_STATE {
	PEG_INITIAL		= 0,
	PEG_FILLED		= 1,
	PEG_EMPTY		= 2,
	PEG_DRAGED		= 3
};

#include <QGraphicsItem>

class Peg;
class QGraphicsEllipseItem;
class Board;

class PegBox : public EmptyBox
{
public:
	PegBox(const QPoint& position, Board* board, QGraphicsItem* parent = 0);
	bool hasPeg() const {return m_peg != 0;}
	Peg* getPeg() const {return m_peg;}
	void setPeg(Peg* peg);
	void setBoard(Board* board) {m_board = board;}
	void setPegState(const PEG_STATE& state = PEG_EMPTY);
	void setPegColor(int color_number);
	int getPegColor();
	void setState(const BOX_STATE& state = BOX_FUTURE);
	bool isPegVisible();


	PEG_STATE getPegState() const {return p_state;}

private:
	PEG_STATE p_state;
	QGraphicsEllipseItem* circle;
	Board* m_board;
	Peg* m_peg;
};

#endif // PEGBOX_H
