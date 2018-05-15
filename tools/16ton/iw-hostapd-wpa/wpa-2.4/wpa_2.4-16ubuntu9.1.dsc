Format: 3.0 (quilt)
Source: wpa
Binary: hostapd, wpagui, wpasupplicant, wpasupplicant-udeb
Architecture: linux-any kfreebsd-any
Version: 2.4-16ubuntu9.1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Stefan Lippers-Hollmann <s.l-h@gmx.de>, Jan Dittberner <jandd@debian.org>
Homepage: http://w1.fi/wpa_supplicant/
Standards-Version: 3.9.6
Vcs-Browser: http://anonscm.debian.org/viewvc/pkg-wpa/wpa/trunk/
Vcs-Svn: svn://anonscm.debian.org/pkg-wpa/wpa/trunk/
Build-Depends: debhelper (>> 9.20120115), libdbus-1-dev, libssl-dev, libqt4-dev, libncurses5-dev, libpcsclite-dev, libnl-3-dev [linux-any], libnl-genl-3-dev [linux-any], libnl-route-3-dev [linux-any], libpcap-dev [kfreebsd-any], libbsd-dev [kfreebsd-any], libreadline-dev, pkg-config, qt4-qmake, docbook-to-man, docbook-utils, android-headers
Package-List:
 hostapd deb net optional arch=linux-any,kfreebsd-any
 wpagui deb net optional arch=linux-any,kfreebsd-any
 wpasupplicant deb net optional arch=linux-any,kfreebsd-any
 wpasupplicant-udeb udeb debian-installer standard arch=linux-any
Checksums-Sha1:
 be9f0c01074cebe981a168eb747eab252eeff5f6 1834600 wpa_2.4.orig.tar.xz
 6e165ed2c94add306f616c482d0ea422c9c3512a 102592 wpa_2.4-16ubuntu9.1.debian.tar.xz
Checksums-Sha256:
 a1e4eda50796b2234a6cd2f00748bbe09f38f3f621919187289162faeb50b6b8 1834600 wpa_2.4.orig.tar.xz
 b8f35e4ad04b16ffffce12b3d779c8934b79fa6f7ec360bb00c10977d4a20c2f 102592 wpa_2.4-16ubuntu9.1.debian.tar.xz
Files:
 6a77b9fe6838b4fca9b92cb22e14de1d 1834600 wpa_2.4.orig.tar.xz
 0e40de539bfe582447e3907bfe3f8d56 102592 wpa_2.4-16ubuntu9.1.debian.tar.xz
Original-Maintainer: Debian wpasupplicant Maintainers <pkg-wpa-devel@lists.alioth.debian.org>
