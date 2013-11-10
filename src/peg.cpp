#include "peg.h"
#include "pegbox.h"
#include <QPen>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QGraphicsTextItem>
#include <QFont>

const QString Peg::color_rgb[10][2] = {
	{"#FFFF80", "#C05800"}, {"#FF3300", "#400040"},
	{"#33CCFF", "#000080"}, {"#808080", "#000000"},
	{"#FFFFFF", "#797979"},{"#FF9900", "#A82A00"},
	{"#66FF33", "#385009"},{"#BA88FF", "#38005D"},
	{"#00FFFF", "#004040"}, {"#FFC0FF", "#800080"},
  };
const char Peg::ordered_chars[10] = {'A', 'B', 'C', 'D', 'E',
								   'F', 'G', 'H', 'I', 'J'};
const char Peg::ordered_digits[10] = {'0', '1', '2', '3', '4',
									'5', '6', '7', '8', '9'};
//const QFont Peg::m_font = Peg::getIndicatorFont();


Peg::Peg(const QPoint& position, int color_number, const int &indicator_t, QGraphicsItem *parent):
	QGraphicsEllipseItem(2.5, 2.5, 34, 34, parent),
	m_position(position),
	m_box(0),
	m_indicator(-1)
{
	m_color = (-1 < color_number && color_number < 10) ? color_number : 0;
	switch (indicator_t) {
	case 1:
		m_indicator = ordered_digits[m_color];
		break;
	case -1:
		m_indicator = ordered_chars[m_color];
		break;
	default:
		m_indicator = ' ';
		break;
	}

	setPen(Qt::NoPen);

	pressedEffect = new QGraphicsDropShadowEffect;
	pressedEffect->setBlurRadius(10);
	pressedEffect->setXOffset(8);
	setGraphicsEffect(pressedEffect);
	pressedEffect->setEnabled(false);

	auto gloss = new QGraphicsEllipseItem(5, 1.5, 24, 20, this);
	gloss->rotate(-45);
	gloss->setPos(-4, 20);
	QLinearGradient lgrad(25, 0, 25, 17);
	lgrad.setColorAt(0, QColor(0xff, 0xff, 0xff, 120));
	lgrad.setColorAt(1, QColor(0xff, 0xff, 0xff, 0x00));
	gloss->setBrush(lgrad);
	gloss->setPen(Qt::NoPen);

	setColor(m_color);
	setZValue(2);
	setPos(m_position.x(), m_position.y());
	setMovable(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	setAcceptDrops(true);
}

Peg::~Peg()
{
}

void Peg::setColor(const int &color_number)
{
	m_color = (-1 < color_number && color_number < 10) ? color_number : 0;
	QRadialGradient gradient(20,20, 40, 5, 5);
	gradient.setColorAt(0, QColor(color_rgb[m_color][0]));
	gradient.setColorAt(1, QColor(color_rgb[m_color][1]));
	setBrush(gradient);
//	update();
}

void Peg::move(QPoint position)
{
	m_position = position;
	setPos(m_position.x(), m_position.y());
}

void Peg::showIndicator()
{

}

void Peg::setMovable(bool enabled)
{
	setFlag(QGraphicsItem::ItemIsMovable, enabled);
	setEnabled(enabled);
	setCursor(enabled ? Qt::OpenHandCursor : Qt::ArrowCursor);
	setZValue(1);
}


void Peg::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (isEnabled())
	{
		pressedEffect->setEnabled(true);
		setZValue(3);
		if(m_box->getPegState() != PEG_INITIAL) m_box->setPegState(PEG_DRAGED);
		setCursor(Qt::ClosedHandCursor);
	}
	QGraphicsEllipseItem::mousePressEvent(event);
}

void Peg::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

	if (isEnabled())
	{
		pressedEffect->setEnabled(false);
		setZValue(2);
		setCursor(Qt::OpenHandCursor);
	}

	QGraphicsEllipseItem::mouseReleaseEvent(event);

	switch (!m_box->sceneBoundingRect().contains(sceneBoundingRect().center().toPoint())) {
	case 0: //	droped on its own box, no need to emit signal and set state to PEG_FILLED
		if (m_box->getPegState() != PEG_INITIAL)
			m_box->setPegState(PEG_FILLED);
		break;
	default: //droped out of its own box,
		if (m_box->getPegState() != PEG_INITIAL)
			m_box->setPegState(PEG_EMPTY);
		emit mouseReleasedSignal(sceneBoundingRect().center().toPoint(), m_color);
		break;
	}


	setPos(m_box->sceneBoundingRect().topLeft());
}

void Peg::onShowIndicators(bool enabled)
{

}
