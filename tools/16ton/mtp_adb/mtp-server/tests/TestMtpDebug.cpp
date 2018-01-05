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
#include "MtpDebug.h"

using namespace android;

BOOST_AUTO_TEST_CASE(DebugGetOperationCodeName)
{
    BOOST_CHECK_EQUAL( MtpDebug::getOperationCodeName(MTP_OPERATION_GET_DEVICE_INFO), "MTP_OPERATION_GET_DEVICE_INFO" );
}

BOOST_AUTO_TEST_CASE(DebugGetFormatCodeName)
{
    BOOST_CHECK_EQUAL( MtpDebug::getFormatCodeName(MTP_FORMAT_PNG), "MTP_FORMAT_PNG" );
}

BOOST_AUTO_TEST_CASE(DebugGetObjectPropCodeName)
{
    BOOST_CHECK_EQUAL( MtpDebug::getObjectPropCodeName(MTP_PROPERTY_STORAGE_ID), "MTP_PROPERTY_STORAGE_ID" );
}

BOOST_AUTO_TEST_CASE(DebugGetDevicePropCodeName)
{
    BOOST_CHECK_EQUAL( MtpDebug::getDevicePropCodeName(MTP_DEVICE_PROPERTY_BATTERY_LEVEL), "MTP_DEVICE_PROPERTY_BATTERY_LEVEL" );
}

BOOST_AUTO_TEST_CASE(DebugGetCodeNameUnknown)
{
    BOOST_CHECK_EQUAL( MtpDebug::getOperationCodeName (1), "UNKNOWN");
}
