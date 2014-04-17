#!/bin/bash

cd ../pkgbuild

mkdir -p arch
cp -u ../QtMind/AUR/*.* arch/
cd arch
find . -name qtmind\*.xz -delete
makepkg -s
linux32 makepkg -src --config ~/.makepkg.i686.conf
cd ../


# fpm -s dir -t deb -a "i386" -n "qtmind" --category 'Qt,Game,BoardGame' --deb-field 'Architecture:amd64'  --deb-field 'Depends: libqtgui4' --description 'A Mastermind game in Qt, with built in solver. Most of the major solving algorithms are integrated in QtMind.' --deb-field 'Provides:qtmind' --url 'http://github.com/omidnikta/qtmind' --license GPL2 --vendor '' -v 0.8.1-1 -m 'Omid Nikta' usr
# 
# 
# fpm -s dir -t deb -a "amd64" -n "qtmind" --category 'Qt,Game,BoardGame' --deb-field 'Architecture:amd64'  --deb-field 'Depends: libqtgui4' --description 'A Mastermind game in Qt, with built in solver. Most of the major solving algorithms are integrated in QtMind.' --deb-field 'Provides:qtmind' --url 'http://github.com/omidnikta/qtmind' --license GPL2 --vendor '' -v 0.8.1-1 -m 'Omid Nikta' usr



mkdir -p win64/qtmind-build
cd win64/qtmind-build
x86_64-w64-mingw32-qmake ../../../QtMind
make clean
make
cp release/qtmind.exe ../
cd ../..

mkdir -p win32/qtmind-build
cd win32/qtmind-build
i686-w64-mingw32-qmake ../../../QtMind
make clean
make
cp release/qtmind.exe ../
cd ../..

rm -r android
mkdir android
cd android
/opt/android-qt5/5.2.0/bin/qmake ../../QtMind/
make install INSTALL_ROOT=android-build/
/opt/android-qt5/5.2.0/bin/androiddeployqt --output android-build/ --sign ../../../android_release.keystore nikta --input android-libqtmind.so-deployment-settings.json

cd ..