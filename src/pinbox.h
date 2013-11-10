#ifndef PINBOX_H
#define PINBOX_H

#include "emptybox.h"
class Pin;

class PinBox : public QObject, public EmptyBox
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	PinBox(const int& pin_number, const QPoint& position, QGraphicsItem* parent = 0);
	static const int pin_positions[5][5][2];
	int getValue(int& blacks, int& whites) const;
	int getValue() const;
	void setPins(const int& b, const int& w);
	void setPins(const QString& codeA, const QString& codeB, const int& peg, const int& color);
	void setState(const BOX_STATE& state = BOX_FUTURE);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
	void pinBoxPushed();

private:
	QVector<Pin*> pins;
};

#endif // PINBOX_H
