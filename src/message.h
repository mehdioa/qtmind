/***********************************************************************
 *
 * Copyright (C) 2013 Omid Nikta <omidnikta@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QGraphicsTextItem>
#include <QTextLayout>

/**
 * @brief The Message class The class represent a message on the board.
 */
class Message : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit Message(const QString& fontName, const int& fontSize, const QString& color_name = "#303133",
                     const int& smaller = 0, QGraphicsItem* parent = 0);
    /**
    * @brief setText set the text of the message
    * @param _text the message text
    */
    void setText(const QString _text);

public slots:
    /**
     * @brief onFontChanged changes the font of the message and update it
     * @param fontName the font name
     * @param fontSize the font size
     */
    void onFontChanged(const QString& fontName, const int& fontSize);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);
    QRectF boundingRect() const;

private:
    QTextLayout mTextLayout; /**< TODO */
    QRectF mUpdateRect; /**< TODO */
    QColor mColor; /**< TODO */
    QString mText; /**< TODO */
    int mFontSizeReduced;
};

#endif // MESSAGE_H
