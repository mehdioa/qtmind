#include "message.h"
#include <QTextLayout>
#include <QPainter>

Message::Message()
{
	m_font = QFont("Arial", 12, QFont::Bold, false);
	m_font.setStyleHint(QFont::SansSerif);
	m_font.setPixelSize(16);

	m_layout.setFont(m_font);
	m_layout.setTextOption(QTextOption(Qt::AlignHCenter));
	m_updateRect = QRectF(0, 0, 10, 10);

}

void Message::showMassage(const QString str)
{
	m_updateRect = boundingRect();
	m_layout.setText(str);

	int leading = -3;
	qreal h = 0;
	qreal maxw = 0;
	qreal maxh = 0;
	m_layout.beginLayout();

	while (true)
	{
		QTextLine line = m_layout.createLine();
		if (!line.isValid())
		{
			break;
		}

		line.setLineWidth(280);
		h += leading;
		line.setPosition(QPointF(0, h));
		h += line.height();
		maxw = qMax(maxw, line.naturalTextWidth());
	}
	m_layout.endLayout();

	float ypos = 4 + (70 - m_layout.boundingRect().height()) / 2;

	maxw = qMax(m_updateRect.width(), m_layout.boundingRect().width());
	maxh = qMax(m_updateRect.height(), m_layout.boundingRect().height() + ypos);

	m_updateRect = QRectF(0, 0, maxw, maxh + ypos);

	update(boundingRect());
}

QRectF Message::boundingRect() const
{
	return m_updateRect;
}

void Message::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setRenderHint(QPainter::TextAntialiasing, true);
	painter->setPen(QPen(QColor("#303133")));
	float ypos = 4 + (70 - m_layout.boundingRect().height()) / 2;
	m_layout.draw(painter, QPointF(0, ypos));
}
