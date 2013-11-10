#ifndef BUTTON_H
#define BUTTON_H

#include <QGraphicsItem>
#include <QFont>
#include <QBrush>
class Board;

class Button : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	Button(int buttonWidth = 152, QString str = "");
//	void setWidth
	void setPushable(bool);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent*);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

	void paint(QPainter* painter, const QStyleOptionGraphicsItem*,
			   QWidget*);
	QRectF boundingRect() const;

signals:
	void buttonPressed();

private:
	Board* gameBoard;
	QString mLabel;
	QFont mFont;
	int mWidth;
	int mYOffs;

	QBrush mFillOutBrush;
	QBrush mFillOverBrush;
	QBrush* mFillBrush;

	QBrush mFrameOutBrush;
	QBrush mFrameOverBrush;
	QBrush* mFrameBrush;

	QRectF mRect;
	QRectF mRectFill;

};

#endif // BUTTON_H
