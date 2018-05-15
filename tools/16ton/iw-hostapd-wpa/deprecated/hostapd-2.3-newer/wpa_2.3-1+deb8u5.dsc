-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: wpa
Binary: hostapd, wpagui, wpasupplicant, wpasupplicant-udeb
Architecture: linux-any kfreebsd-any
Version: 2.3-1+deb8u5
Maintainer: Debian wpasupplicant Maintainers <pkg-wpa-devel@lists.alioth.debian.org>
Uploaders: Kel Modderman <kel@otaku42.de>, Stefan Lippers-Hollmann <s.l-h@gmx.de>, Jan Dittberner <jandd@debian.org>
Homepage: http://w1.fi/wpa_supplicant/
Standards-Version: 3.9.6
Vcs-Browser: http://anonscm.debian.org/viewvc/pkg-wpa/wpa/trunk/
Vcs-Svn: svn://anonscm.debian.org/pkg-wpa/wpa/trunk/
Build-Depends: debhelper (>> 9.20120115), libdbus-1-dev, libssl-dev, libqt4-dev, libncurses5-dev, libpcsclite-dev, libnl-3-dev [linux-any], libnl-genl-3-dev [linux-any], libnl-route-3-dev [linux-any], libpcap-dev [kfreebsd-any], libbsd-dev [kfreebsd-any], libreadline-dev, pkg-config, qt4-qmake, docbook-to-man, docbook-utils
Package-List:
 hostapd deb net optional arch=linux-any,kfreebsd-any
 wpagui deb net optional arch=linux-any,kfreebsd-any
 wpasupplicant deb net optional arch=linux-any,kfreebsd-any
 wpasupplicant-udeb udeb debian-installer standard arch=linux-any
Checksums-Sha1:
 7737a4306195ffaba8bb6777e2ede5a4a25e3ca0 1735544 wpa_2.3.orig.tar.xz
 7f2e644aff9fdf766cb00e612e0ec98e3d3cb806 92112 wpa_2.3-1+deb8u5.debian.tar.xz
Checksums-Sha256:
 3d96034fa9e042c8aacb0812d8b2ab3d4c9aa6fc410802b4ee0da311e51c3eb3 1735544 wpa_2.3.orig.tar.xz
 5b5021d7a5e023a5e4ad464de974ad60194e80d04192db96408e52cf2290baf3 92112 wpa_2.3-1+deb8u5.debian.tar.xz
Files:
 d6dc9fa32a406506717ee6a4d076cd6d 1735544 wpa_2.3.orig.tar.xz
 da8b95455ab7eded8ece38fdc16fe7a3 92112 wpa_2.3-1+deb8u5.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQEzBAEBCgAdFiEEl0WwInMjgf6efq/1bdtT8qZ1wKUFAlniGrIACgkQbdtT8qZ1
wKUBxwf/S8Wmb4xDqzkhVD53hPYPBrljeBd6xDtB8yDYSOom8Jni9Sn2p5FUzy1t
comi4RwyHX9uqGAFw6Dx7msnaEE8mdRZlHPRS5Av9oCN9a8JjK2nBNHSqyabVF4E
Gw7ytsYgWsOR7+EaHZGUaaaPh8rVL5N52pjsJHdiX92wobHceMM7HH+k+YQ/TdAW
n8EFthCSNT+wfQoHCrH2lq9YI/jIn64BsjixxVwF9KQewd2OyUV6bdD8GL8YCMfu
MhCNCc+ZDT/D+2Yr1f7RS1El8uWx0mJSc+WZmZ+V9JXGO8vaQwZZw3aeweANJdDw
m58EOHk2ZqDt7ABCBcQzOq7V5zYbfg==
=tYDP
-----END PGP SIGNATURE-----
