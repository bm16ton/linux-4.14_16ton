# Makefile for adbd; based on core/adb/Android.mk

VPATH+= ../adb
SRCS+=  adb.c
SRCS+=  fdevent.c
SRCS+=  transport.c
SRCS+=  transport_local.c
SRCS+=  transport_usb.c
SRCS+=  adb_auth_client.c
SRCS+=  sockets.c
SRCS+=  services.c
SRCS+=  file_sync_service.c
SRCS+=  jdwp_service.c
SRCS+=  framebuffer_service.c
SRCS+=  remount_service.c
SRCS+=  disable_verity_service.c
SRCS+=  usb_linux_client.c

VPATH+= ../libcutils
SRCS+= socket_inaddr_any_server.c
SRCS+= socket_local_client.c
SRCS+= socket_local_server.c
SRCS+= socket_loopback_client.c
SRCS+= socket_loopback_server.c

VPATH+= ../liblog
SRCS+= logd_write_kern.c

VPATH+= ../libmincrypt
SRCS+= rsa.c
SRCS+= sha.c
SRCS+= sha256.c

VPATH+= ../ubuntu
SRCS+= selinux_stub.c
SRCS+= ubuntu_sockets.c

CPPFLAGS+= -O2 -g -Wall -Wno-unused-parameter
CPPFLAGS+= -DADB_HOST=0 -DHAVE_FORKEXEC=1 -D_XOPEN_SOURCE -D_GNU_SOURCE -DALLOW_ADBD_ROOT=1
CPPFLAGS+= -DHAVE_SYMLINKS
CPPFLAGS+= -DHAVE_TERMIO_H
CPPFLAGS+= -I.
CPPFLAGS+= -I../adb
CPPFLAGS+= -I../include
CPPFLAGS+= -I../ubuntu
CPPFLAGS+= -I../../../external/zlib
CPPFLAGS+= `pkg-config --cflags glib-2.0 gio-2.0`

LIBS+= -lc -lpthread -lz -lcrypto -landroid-properties -lsystemd -lresolv `pkg-config --libs glib-2.0 gio-2.0`

OBJS= $(patsubst %, %.o, $(basename $(SRCS)))

all: adbd

adbd: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)

clean:
	rm -rf $(OBJS) adbd
