# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="4"

DESCRIPTION="Is a computer clone of the mastermind game with qt library"
HOMEPAGE="http://opendesktop.org/content/show.php/QtMind?content=162733"
SRC_URI="https://github.com/omidnikta/qtmind/archive/master.zip"

LICENSE="GPL"
SLOT="0"

inherit eutils kde4-base qt4-r2

KEYWORDS="amd64 ~mips x86"

RDEPEND=">=kde-base/oxygen-icons-4.4:4[aqua=]"
DEPEND="${RDEPEND}
    dev-qt/qtcore:4
    >=kde-base/kdelibs-4.4:4[aqua=]"
    
S=${WORKDIR}/${PN}-master
RESTRICT="strip"

src_configure(){
    echo "${WORKDIR}"
    cd "${S}"
    #kde4-base_src_configure
    #cmake .
    #make 
    eqmake4 ${PN}.pro
    #make clean && make QMAKE=qmake-qt4
    #./configure.sh --prefix=/usr && make QMAKE=qmake-qt4
    #sed -i "9s|Application;||g" "${S}"/build/hotshots.desktop || die "sed failed"
    #sed -i -e '2d' "${S}"/build/hotshots.desktop || die "sed failed"
}

	
src_install() {
    cd "${S}"
    echo "${WORKDIR}"
    #emake DESTDIR="${D}" install || die "Install failed!"
    #kde4-base_src_install
    qt4-r2_src_install
    #make install INSTALL_ROOT="${D}"/usr
    #dodir /usr/{bin,share}
    #dodir /usr/share/{applications,knotter,doc}
    #cp "${S}"/${PN} "${D}"/usr/bin
    #cp "${S}"/${PN}.desktop "${D}"usr/share/applications
    #cp -pPR "${S}"/data/* "${D}"usr/share/knotter/
    #cp -pPR "${S}"/man "${D}"usr/share
    #cp -pPR "${S}"/doc "${D}"usr/share
    #rm -R "${D}"var || die
}
