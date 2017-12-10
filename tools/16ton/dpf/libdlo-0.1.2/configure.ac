#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([libdlo], [0.1.2], [libdlo@displaylink.com])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2])
AC_CONFIG_SRCDIR([src/dlo_grfx.c])
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# create LIBTOOLS macros.
LT_INIT

# See LIBTOOLS on what these mean
LIBDLO_LT_CURRENT=1
LIBDLO_LT_REVISION=0
LIBDLO_LT_AGE=1
AC_SUBST(LIBDLO_LT_CURRENT)
AC_SUBST(LIBDLO_LT_REVISION)
AC_SUBST(LIBDLO_LT_AGE)

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h usb.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_LIB(usb,usb_open)
AC_CHECK_FUNC([usb_get_driver_np],,[AC_MSG_ERROR([Can't find libusb. On ubuntu, try sudo apt-get install libusb-dev])])
AC_CHECK_FUNC([usb_get_configuration],[AC_MSG_ERROR([libdlo currently uses libusb-0.12 or 0.13. You appear to have 1.0])]) 
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday strchr])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 test/Makefile
                ])
AC_OUTPUT
AC_MSG_RESULT([
	libdlo $VERSION
	========
	
	prefix:			${prefix}
	exec_prefix:		${exec_prefix}
	libdir_name:		${libdir}
	mandir:			${mandir}
	includedir:		${includedir}
						
	compiler:		${CC}
	cflags:			${CFLAGS}
	ldflags:		${LDFLAGS}
								
	xsltproc:		${XSLTPROC}
])

