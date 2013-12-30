#include "boardfont.h"
#include "QSettings"

BoardFont::BoardFont():
	fontName(QSettings().value("FontName", "SansSerif").toString()),
	fontSize((QSettings().value("FontSize", 12).toInt()))
{

}

BoardFont::~BoardFont()
{
	QSettings().setValue("FontName", fontName);
	QSettings().setValue("FontSize", fontSize);
}
