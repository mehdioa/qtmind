#ifndef EmptyBox_H
#define EmptyBox_H
enum BOX_STATE {
	BOX_PAST		= 0,
	BOX_CURRENT		= 1,
	BOX_FUTURE		= 2,
	BOX_NONE		= 3
};
#include <QGraphicsItem>

class EmptyBox : public QGraphicsItem
{
//	Q_OBJECT
//	Q_INTERFACES(QGraphicsItem)

public:
	EmptyBox(const QPoint& position = QPoint(0, 0), QGraphicsItem* parent = 0);

	virtual void setState(const BOX_STATE& state = BOX_FUTURE);

	BOX_STATE getState() const {return m_state;}

	void setHighlight();

//	virtual void handlePegMouseRelease(QPoint){}
//	virtual bool hasPeg() const { return false;}
//	virtual void setPegColor(int ){}
	QRectF boundingRect() const;

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

	BOX_STATE m_state;
	QColor mPend;
	QColor mPenl;
	QColor mGrad0;
	QColor mGrad1;
};

#endif // EmptyBox_H
