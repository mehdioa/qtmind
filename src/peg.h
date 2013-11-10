#ifndef PEG_H
#define PEG_H

#include <QGraphicsEllipseItem>

class QGraphicsDropShadowEffect;
class PegBox;

class Peg : public QObject, public QGraphicsEllipseItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)
public:
	const static QString color_rgb[10][2];
	const static char ordered_chars[10];
	const static char ordered_digits[10];

	Peg(const QPoint& position, int color_number = 0, const int &indicator_t = 0, QGraphicsItem* parent = 0);
	~Peg();
	void setColor(const int& color_number);
	int getColor() const {return m_color;}
	void move (QPoint);
	void showIndicator();
	void setMovable(bool );
	void setPegBox(PegBox* box) {m_box = box;}


protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *);

signals:
	void mouseReleasedSignal(QPoint position, int color);

protected slots:
	void onShowIndicators(bool enabled);

private:
	QPointF m_position;// position of hole containing peg
	PegBox* m_box;
//	QFont m_font;
//	QFont getIndicatorFont() const;

	QGraphicsDropShadowEffect* pressedEffect;
	/* pressed shadow - the ellipse item takes ownership of
	 *the effect, so no need to delete the pointer in the destructor*/
	int m_color;
	char m_indicator;
};

#endif // PEG_H
