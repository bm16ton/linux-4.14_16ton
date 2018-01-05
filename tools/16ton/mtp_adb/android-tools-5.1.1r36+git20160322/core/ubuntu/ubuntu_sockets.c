/*
 * Copyright © 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */

#include <stdio.h>
#include <string.h>
#include <resolv.h>
#include <cutils/list.h>
#include <cutils/sockets.h>
#include <systemd/sd-daemon.h>
#include <sys/un.h>
#include <stdlib.h>

#include "sysdeps.h"
#include "adb_trace.h"

#include "ubuntu_sockets.h"

#define TRACE_TAG TRACE_AUTH

/**
 * Create android socket
 */
int ubuntu_create_android_control_socket(const char *name, int type, mode_t perm, uid_t uid, gid_t gid) {
    int fd, n;
    D("ubuntu_get_android_control_socket<<<\n");
    n = sd_listen_fds(0);
    if (n > 1) {
        D("Too many file descriptors received.\n");
        return -1;
    } else if (n == 1) {
        fd = SD_LISTEN_FDS_START + 0;
        D("Socket already exists: fd=%d\n", fd);
    } else {
        union {
                struct sockaddr sa;
                struct sockaddr_un un;
        } sa;

        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0) {
            D("socket(): %m\n", stderr);
            return -1;
        }

        memset(&sa, 0, sizeof(sa));
        sa.un.sun_family = AF_UNIX;
        snprintf(sa.un.sun_path, sizeof(sa.un.sun_path), ANDROID_SOCKET_DIR"/%s", name);

        if (bind(fd, &sa.sa, sizeof(sa)) < 0) {
                D("bind(): %m\n", stderr);
                return -1;
        }

        chown(sa.un.sun_path, uid, gid);
        chmod(sa.un.sun_path, perm);
        char key[64] = ANDROID_SOCKET_ENV_PREFIX;
        char val[64];

        strncpy(key + sizeof(ANDROID_SOCKET_ENV_PREFIX) - 1,
                name,
                sizeof(key) - sizeof(ANDROID_SOCKET_ENV_PREFIX));
        snprintf(val, sizeof(val), "%d", fd);
        setenv(key, val, 1);
    }
    return fd;
}
