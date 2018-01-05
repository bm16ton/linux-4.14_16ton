/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <cstdlib>

// #include <cutils/tztime.h>
#include "MtpUtils.h"

using namespace android;

BOOST_AUTO_TEST_CASE(UtilsParseDateTime)
{
    time_t seconds;

    setenv("TZ", "UTC", 1);

    parseDateTime("20130909T114143", seconds);
    BOOST_CHECK_EQUAL(seconds, 1378726903l);
}

BOOST_AUTO_TEST_CASE(UtilsFormatDateTime)
{
    time_t seconds = 1378726903;
    char buffer[25];
    const char *expected = "20130909T114143";

    setenv("TZ", "UTC", 1);

    formatDateTime(seconds, buffer, 25);
    BOOST_CHECK_EQUAL(strcmp(expected, buffer), 0);
}
