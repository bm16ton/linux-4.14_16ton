AC_INIT([xtables-addons], [2.12])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_INSTALL
AM_INIT_AUTOMAKE([1.10b -Wall foreign subdir-objects])
AC_PROG_CC
AM_PROG_CC_C_O
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_ARG_WITH([kbuild],
	AS_HELP_STRING([--with-kbuild=PATH],
	[Path to kernel build directory [[/lib/modules/CURRENT/build]]])
AS_HELP_STRING([--without-kbuild],
	[Build only userspace tools]),
	[kbuilddir="$withval"],
	[kbuilddir="/lib/modules/$(uname -r)/build"])
#
# check for --without-kbuild
#
if [[ "$kbuilddir" == no ]]; then
	kbuilddir="";
fi

AC_CHECK_HEADERS([linux/netfilter/x_tables.h], [],
	[AC_MSG_ERROR([You need to have linux/netfilter/x_tables.h, see INSTALL file for details])])
PKG_CHECK_MODULES([libxtables], [xtables >= 1.4.5])
xtlibdir="$(pkg-config --variable=xtlibdir xtables)"

AC_ARG_WITH([xtlibdir],
	AS_HELP_STRING([--with-xtlibdir=PATH],
	[Path where to install Xtables extensions [[autodetect]]]),
	[xtlibdir="$withval"])
AC_MSG_CHECKING([Xtables module directory])
AC_MSG_RESULT([$xtlibdir])

regular_CPPFLAGS="-D_LARGEFILE_SOURCE=1 -D_LARGE_FILES -D_FILE_OFFSET_BITS=64 \
	-D_REENTRANT -I\${XA_TOPSRCDIR}/include"
regular_CFLAGS="-Wall -Waggregate-return -Wmissing-declarations \
	-Wmissing-prototypes -Wredundant-decls -Wshadow -Wstrict-prototypes \
	-Winline -pipe";

if test -n "$kbuilddir"; then
	AC_MSG_CHECKING([kernel version that we will build against])
	krel="$(make -sC "$kbuilddir" M=$PWD kernelrelease | $AWK -v 'FS=[[^0-9.]]' '{print $1; exit}')"
	save_IFS="$IFS"
	IFS='.'
	set x $krel
	IFS="$save_IFS"
	kmajor="$(($2+0))"
	kminor="$(($3+0))"
	kmicro="$(($4+0))"
	kstable="$(($5+0))"
	if test -z "$kmajor" -o -z "$kminor" -o -z "$kmicro"; then
		echo "WARNING: Version detection did not succeed. Continue at own luck.";
	else
		echo "$kmajor.$kminor.$kmicro.$kstable in $kbuilddir";
		if test "$kmajor" -gt 4 -o "$kmajor" -eq 4 -a "$kminor" -gt 10; then
			echo "WARNING: That kernel version is not officially supported yet. Continue at own luck.";
		elif test "$kmajor" -eq 4 -a "$kminor" -le 10; then
			:;
		elif test "$kmajor" -eq 3 -a "$kminor" -ge 7; then
			:;
		else
			echo "WARNING: That kernel version is not officially supported.";
		fi;
	fi;
fi;

AC_SUBST([regular_CPPFLAGS])
AC_SUBST([regular_CFLAGS])
AC_SUBST([kbuilddir])
AC_SUBST([xtlibdir])
AC_CONFIG_FILES([Makefile Makefile.iptrules Makefile.mans geoip/Makefile
	extensions/Makefile extensions/ACCOUNT/Makefile
	extensions/pknock/Makefile])
AC_OUTPUT
