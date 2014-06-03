#!/bin/bash

cd ../pkgbuild

mkdir -p arch

mkdir -p arch/src

cp -u ../QtMind/AUR/* arch/

cd arch

find . -name qtmind\*.xz -delete

find . -name qtmind\*.rpm -delete

find . -name qtmind\*.deb -delete

rm -r usr/

cd src
	if [ -d master ] ; then
		cd master && git pull origin master && cd ..
		msg "The local files are updated."
	else
		git clone ../../../QtMind --single-branch master
	fi

cd ..

makepkg -s
# linux32 makepkg -src --config ~/.makepkg.i686.conf

mv qtmind*.xz qtmind.xz

tar xf qtmind.xz

rm .INSTALL .MTREE .PKGINFO qtmind.xz

fpm -s dir -t deb -a "amd64" -n "qtmind" --category 'Qt,Game,BoardGame' --deb-field  'Depends: libqtgui4' --description 'A Mastermind game in Qt, with built in solver. Most of the major solving algorithms are integrated in QtMind.' --deb-field 'Provides:qtmind' --url  'http://github.com/omidnikta/qtmind' --license GPL2 --vendor '' -v $1 -m 'Omid Nikta' usr

mv -f qtmind*_amd64.deb ../../gh-pages/Downloads/qtmind_amd64.deb

fpm -s dir -t rpm -n "qtmind" --category 'Qt,Game,BoardGame' --description 'A Mastermind game in Qt, with built in solver. Most of the major solving algorithms are integrated in QtMind.'  --url 'http://github.com/omidnikta/qtmind' --license GPL2 --vendor '' -v $1 -m 'Omid Nikta' -d 'libqt4' usr

mv -f qtmind-*.x86_64.rpm ../../gh-pages/Downloads/qtmind_suse.x86_64.rpm

fpm -s dir -t rpm -n "qtmind" --category 'Qt,Game,BoardGame' --description 'A Mastermind game in Qt, with built in solver. Most of the major solving algorithms are integrated in QtMind.'  --url 'http://github.com/omidnikta/qtmind' --license GPL2 --vendor '' -v $1 -m 'Omid Nikta' -d 'qt-x11' usr

mv -f qtmind-*.x86_64.rpm ../../gh-pages/Downloads/qtmind_fedora.x86_64.rpm
cd ..

mkdir -p win64

mkdir -p win64/qtmind-build

cd win64/qtmind-build

x86_64-w64-mingw32-qmake-qt5 ../../../QtMind

make clean

make

cp release/qtmind.exe ../

cd ../..

mkdir -p win32

mkdir -p win32/qtmind-build

cd win32/qtmind-build

i686-w64-mingw32-qmake-qt5 ../../../QtMind

make clean

make

cp release/qtmind.exe ../

cd ../..

#rm -r android
mkdir -p android

cd android

/opt/android-qt5/5.3.0/bin/qmake ../../QtMind/

make install INSTALL_ROOT=android-build/

/opt/android-qt5/5.3.0/bin/androiddeployqt --output android-build/ --sign ../../../android_release.keystore nikta --input android-libqtmind.so-deployment-settings.json

mv -f android-build/bin/QtApp-release.apk ../../../../gh-pages/Downloads/QtMind.apk

