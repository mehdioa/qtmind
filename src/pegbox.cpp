#include "pegbox.h"
#include "peg.h"
#include "board.h"
#include "QGraphicsEllipseItem"
#include <QPen>
#include <QLinearGradient>

PegBox::PegBox(const QPoint &position, Board *board, QGraphicsItem *parent):
	EmptyBox(position, parent),
	p_state(PEG_EMPTY),
	m_board(board),
	m_peg(0)
{
	QLinearGradient lgrad(2, 2, 35, 35);
	lgrad.setColorAt(0.0, QColor(0, 0, 0, 150));
	lgrad.setColorAt(1.0, QColor(0xff, 0xff, 0xff, 0xa0));

	circle = new QGraphicsEllipseItem(2, 2, 35, 35, this);
	circle->setPen(QPen(QBrush(lgrad), 1.5));
	setState();
}


void PegBox::setPeg(Peg *peg)
{
	m_peg = peg;
	m_peg->setPegBox(this);
	circle->setVisible(hasPeg());
}



void PegBox::setPegState(const PEG_STATE &state)
{
	p_state = state;
	switch (p_state) {
	case PEG_INITIAL:
		if (m_peg) m_peg->setVisible(true);
		circle->setVisible(true);
		break;
	case PEG_FILLED:
		if (m_peg) m_peg->setVisible(true);
		circle->setVisible(true);
		break;
	case PEG_EMPTY:
		if (m_peg) m_peg->setVisible(false);
		circle->setVisible(false);
		break;
	default:// PEG_DRAGED
		if (m_peg) m_peg->setVisible(true);
		circle->setVisible(false);
		break;
	}
}

void PegBox::setPegColor(int color_number)
{
	if (hasPeg())
		m_peg->setColor(color_number);
}

int PegBox::getPegColor()
{
	if (hasPeg()) return m_peg->getColor();
	return -1;
}

void PegBox::setState(const BOX_STATE &state)
{
	m_state = state;
	setHighlight();

	switch (m_state) {
	case BOX_PAST:
		setPegState(PEG_FILLED);
		setEnabled(false);
		if (m_peg) {
			m_peg->setEnabled(false);
			m_peg->setMovable(false);
			m_peg->setVisible(true);
		}
		break;
	case BOX_CURRENT:
		setEnabled(true);
		if (m_peg) {
			m_peg->setEnabled(true);
			m_peg->setMovable(true);
			m_peg->setVisible(true);
		}
		break;
	case BOX_FUTURE:
		setEnabled(false);
		if (m_peg) {
			m_peg->setEnabled(false);
			m_peg->setMovable(false);
			m_peg->setVisible(true);
		}
		break;
	default: //BOX_NONE
		setEnabled(false);
		if (m_peg) {
			m_peg->setVisible(false);
		}
		break;
	}
	update();}

bool PegBox::isPegVisible()
{
	return m_peg && m_peg->isVisible();
}

//int PegBox::handlePegMouseRelease(QPoint position)
//{
//	if (sceneBoundingRect().contains(position)) // do nothing if dropped on its own box
//		return 0;
//	else
//		return m_board->handlePegMouseRelease(position, m_peg->getColor());
//}
