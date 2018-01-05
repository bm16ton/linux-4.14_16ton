# Makefile for fastboot; based on core/fastboot/Android.mk

VPATH+= ../fastboot
SRCS+= protocol.c
SRCS+= engine.c
SRCS+= bootimg.c
SRCS+= fastboot.c
SRCS+= util.c
SRCS+= fs.c
SRCS+= usb_linux.c
SRCS+= util_linux.c

VPATH+= ../libsparse
SRCS+= backed_block.c
SRCS+= sparse_crc32.c
SRCS+= sparse.c
SRCS+= sparse_read.c
SRCS+= sparse_err.c
SRCS+= output_file.c

VPATH+= ../../extras/ext4_utils/
SRCS+= make_ext4fs.c
SRCS+= ext4fixup.c
SRCS+= ext4_utils.c
SRCS+= allocate.c
SRCS+= contents.c
SRCS+= extent.c
SRCS+= indirect.c
SRCS+= uuid.c
SRCS+= sha1.c
SRCS+= wipe.c
SRCS+= crc16.c
SRCS+= ext4_sb.c

VPATH+= ../../extras/f2fs_utils/
SRCS+= f2fs_utils.c
SRCS+= f2fs_ioutils.c
SRCS+= f2fs_dlutils.c

VPATH+= ../libzipfile
SRCS+= zipfile.c
SRCS+= centraldir.c

CPPFLAGS+= -DANDROID
CPPFLAGS+= -DHOST=1
CPPFLAGS+= -D_FILE_OFFSET_BITS=64
CPPFLAGS+= -std=gnu99
CPPFLAGS+= -I.
CPPFLAGS+= -I/usr/include
CPPFLAGS+= -I../fastboot
CPPFLAGS+= -I../include
CPPFLAGS+= -I../mkbootimg
CPPFLAGS+= -I../../extras/ext4_utils/
CPPFLAGS+= -I../../extras/f2fs_utils
CPPFLAGS+= -I../include/f2fs-tools
CPPFLAGS+= -I../libsparse/include/
CPPFLAGS+= -I../ubuntu
LIBS+= -lz -ldl -lselinux

OBJS= $(SRCS:.c=.o)

all: fastboot

fastboot: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)

clean:
	rm -rf $(OBJS) fastboot
