-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: wpa
Binary: hostapd, wpagui, wpasupplicant, wpasupplicant-udeb
Architecture: linux-any kfreebsd-any
Version: 2.4-0ubuntu9.1
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
 a5069fabac12b3ed6573182fbd1be26dd6488a73 100960 wpa_2.4-0ubuntu9.1.debian.tar.xz
Checksums-Sha256:
 a1e4eda50796b2234a6cd2f00748bbe09f38f3f621919187289162faeb50b6b8 1834600 wpa_2.4.orig.tar.xz
 1f81764a8a792fc282882215f3d3e325e58acf293b45a5b6b5f6d8796f888992 100960 wpa_2.4-0ubuntu9.1.debian.tar.xz
Files:
 6a77b9fe6838b4fca9b92cb22e14de1d 1834600 wpa_2.4.orig.tar.xz
 7eecb55df5fa711898ea0967f0f14f12 100960 wpa_2.4-0ubuntu9.1.debian.tar.xz
Original-Maintainer: Debian wpasupplicant Maintainers <pkg-wpa-devel@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iQIcBAEBCgAGBQJZ5KmOAAoJEGVp2FWnRL6TSwkQAJQrV1BsVg81J104tIGwFVl+
NPuYLVY7DotvyldYCp1nf4cOYrR5RP9empZZ2zJ2O9cHky5OvxmpCXO+sIrLjI08
GHo6nzVkw20zbnbahicWOTFHxnGN/x7UElX5GmPzabTHSvgZvkHm2DnNU+VzITpb
9KM5n379d8M8tAHZy20idYceLVI1/7I1eDAkZEhQu21zb6oS+6rdGCZyE1XYQh+9
9/acsr0W8MOjWLPE0jOpCY/nBeDIF67xhhbXX5CXu2AalTn+0ks/Y1fpnwuFkTn6
Zn7V/Hj4FO9K+73NNKskTTxT6haJELvDg/GTilsKKUegCDOOkJzy3ssDm/0nUPJ4
WFTwODOo2SZ/MHWqeUU8pEO1aSU7Gui3kyvyJpwNQZAo/M9AJIhKRCoGVobMJ0Yh
RgpUOLh8LLK1To4uh1YMlmk8iXz56Ju9DNXbrwplzKbBoFLe8xabTdgbVFlzZJAH
9BG0Z2nCSxokfHLD+bKL55p73FzHPSkaWN+SPix9jtS9Ga6IwcgxMVXCC67kEbcd
BTpDdLGF5JFUqNjzKY+cepRaxksal+VvsELel6ikvUAH9B7K9aNjkGY/8bZfPycK
FXQDRzT+89ywjEhnAVDXChH2ZiynrARovXR7toqYMdVB49VhloOf+sfvUN5wWTiN
jk7QzQ5C0OuNDi4cYEd0
=IfS1
-----END PGP SIGNATURE-----
