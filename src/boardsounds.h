#ifndef BOARDSOUNDS_H
#define BOARDSOUNDS_H

class QSoundEffect;

class BoardSounds
{
public:
	BoardSounds();
	~BoardSounds();
	void playPegDropSound();
	void playPegDropRefuseSound();
	void playButtonPressSound();
	void setVolume(const int &vol);
	int getVolume() const {return volume;}

private:
	int volume;

//#if QT_VERSION >= 0x050000
	QSoundEffect *pegDropSound;
	QSoundEffect *pegDropRefuseSound;
	QSoundEffect *buttonPressSound;
//#endif
};

#endif // BOARDSOUNDS_H
