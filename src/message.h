#ifndef MESSAGE_H
#define MESSAGE_H
#include <QGraphicsTextItem>
#include <QFont>
#include <QTextLayout>

class QTextLayout;

class Message : public QGraphicsTextItem
{
public:
	Message();
	void showMassage(const QString str);
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
	QTextLayout m_layout;
	QFont m_font;
	QRectF m_updateRect;
};

#endif // MESSAGE_H
