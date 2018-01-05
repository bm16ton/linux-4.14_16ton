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

#include "mtp.h"
#include "MtpPacket.h"

using namespace android;

BOOST_AUTO_TEST_CASE(Packet)
{
    MtpPacket *p = new MtpPacket (4);

    BOOST_REQUIRE(p);
}

BOOST_AUTO_TEST_CASE(PacketReset)
{
    MtpPacket *p = new MtpPacket (MTP_CONTAINER_PARAMETER_OFFSET + 4);
    uint32_t value = UINT32_MAX;
    uint32_t result;

    BOOST_REQUIRE(p);

    p->setParameter(1, value);
    result = p->getParameter(1);
    BOOST_CHECK_EQUAL (value, result);

    p->reset();

    result = p->getParameter(1);
    BOOST_CHECK (value != result);
    BOOST_CHECK (result == 0);
}
