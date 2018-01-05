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

#include <selinux/selinux.h>

// Ubuntu does not use selinux, stub here functions we do not need

/**
 * is selinux enabled
 * stub implementation
 */
int is_selinux_enabled()
{
    return 0;
}

/**
 * selinux restorecon
 * stub implementation
 */
int selinux_android_restorecon(const char * file, unsigned int flags)
{
    return 0;
}

/**
 * Set the current security context to con.
 * stub implementation
 */
int setcon(const char * con)
 {
     return 0;
 }
