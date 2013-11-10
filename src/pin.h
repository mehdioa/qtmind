#ifndef PIN_H
#define PIN_H
#include <QGraphicsEllipseItem>

enum PIN_MOUSE{
	MOUSE_IGNORE	= 0,
	MOUSE_ACCEPT	= 1,
	MOUSE_TOBOX		= 2
};

class Pin : public QGraphicsEllipseItem
{
public:
	Pin(const int &color = 0, QGraphicsItem* parent = 0);
	int getColor() const {return m_color;}
	void setColor(const int& c);
	void setMouseEventHandling(PIN_MOUSE event);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
private:
	int m_color;//  -1 = white, 0 = none, 1 = black
};

#endif // PIN_H
