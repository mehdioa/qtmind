#ifndef PEG_H
#define PEG_H

#include <QGraphicsEllipseItem>

enum INDICATOR_TYPE{
	TYPE_NONE	= 0,
	TYPE_CHAR	= 1,
	TYPE_DIGIT	= 2
};

class QGraphicsDropShadowEffect;
class QGraphicsSimpleTextItem;
class PegBox;

class Peg : public QObject, public QGraphicsEllipseItem
{
		Q_OBJECT
		Q_INTERFACES(QGraphicsItem)
public:
		const static QString color_rgb[10][2];
		const static QString ordered_chars[3];

		Peg(const QPoint& position, int color_number = 0,
			const INDICATOR_TYPE &indicator_t = TYPE_NONE, QGraphicsItem* parent = 0);
		void setColor(const int& color_number);
		int getColor() const {return mColor;}
		void setMovable(bool );
		void setPegBox(PegBox* box) {mBox = box;}

protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *);

signals:
		void mouseReleasedSignal(QPoint position, int color);

protected slots:
		void onChangeIndicators(const INDICATOR_TYPE indicator_t);

private:
		QPointF mPosition;// position of hole containing peg
		PegBox* mBox;
		INDICATOR_TYPE mIndicatorType;

		QGraphicsDropShadowEffect* pressedEffect;
		QGraphicsSimpleTextItem* mIndicator;
		/* pressed shadow - the ellipse item takes ownership of
		 *the effect, so no need to delete the pointer in the destructor*/
		int mColor;
//		char mIndicator;
};

#endif // PEG_H
