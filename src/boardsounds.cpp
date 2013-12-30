#include "boardsounds.h"
#include "QSettings"

#if QT_VERSION >= 0x050000
#include <QtMultimedia/QSoundEffect>
#endif

BoardSounds::BoardSounds()
{
#if QT_VERSION >= 0x050000
	pegDropSound = new QSoundEffect;
	pegDropRefuseSound = new QSoundEffect;
	buttonPressSound = new QSoundEffect;
	pegDropSound->setSource(QUrl::fromLocalFile("://sounds/pegdrop.wav"));
	pegDropRefuseSound->setSource(QUrl::fromLocalFile("://sounds/pegrefuse.wav"));
	buttonPressSound->setSource(QUrl::fromLocalFile("://sounds/pin.wav"));
#endif
	setVolume(QSettings().value("Volume", 50).toInt());
}

BoardSounds::~BoardSounds()
{
	QSettings().setValue("Volume", volume);
#if QT_VERSION >= 0x050000
	delete pegDropSound;
	delete pegDropRefuseSound;
	delete buttonPressSound;
#endif
}

void BoardSounds::playPegDropSound()
{
#if QT_VERSION >= 0x050000
	pegDropSound->play();
#endif
}

void BoardSounds::playPegDropRefuseSound()
{
#if QT_VERSION >= 0x050000
	pegDropRefuseSound->play();
#endif
}

void BoardSounds::playButtonPressSound()
{
#if QT_VERSION >= 0x050000
	buttonPressSound->play();
#endif
}

void BoardSounds::setVolume(const int &vol)
{
	volume = vol;

#if QT_VERSION >= 0x050000
	qreal real_volume = (qreal) volume/100;

	pegDropSound->setVolume(real_volume);
	pegDropRefuseSound->setVolume(real_volume);
	buttonPressSound->setVolume(real_volume);
#endif
}
