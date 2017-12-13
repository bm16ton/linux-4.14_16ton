
rootlibdir = $(libdir)
rootlib_LTLIBRARIES = \
	libdlo.la

include_HEADERS = \
	libdlo.h

libdlo_la_SOURCES = \
	dlo_base.h \
	dlo_data.h \
	dlo_defs.h \
	dlo_grfx.h \
	dlo_mode.h \
	dlo_structs.h \
	dlo_usb.h \
	dlo_grfx.c \
	dlo_mode.c \
	dlo_usb.c  \
	libdlo.c

libdlo_la_CFLAGS = 
libdlo_la_LDFLAGS = \
	-lusb \
	-version-info $(LIBDLO_LT_CURRENT):$(LIBDLO_LT_REVISION):$(LIBDLO_LT_AGE)

