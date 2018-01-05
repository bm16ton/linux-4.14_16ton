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
#include "MtpProperty.h"
#include "MtpStringBuffer.h"

using namespace android;

BOOST_AUTO_TEST_CASE(PropertyConstructorString)
{
    MtpProperty *prop = new MtpProperty(MTP_PROPERTY_NAME, MTP_TYPE_STR, false);

    BOOST_CHECK (prop);
    BOOST_CHECK (prop->mType == MTP_TYPE_STR);
    BOOST_CHECK (prop->mCode == MTP_PROPERTY_NAME);
    BOOST_CHECK (prop->mWriteable == false);

    BOOST_CHECK (prop->mDefaultValue.u.u64 == 0);
    BOOST_CHECK (prop->mCurrentValue.u.u64 == 0);
}

BOOST_AUTO_TEST_CASE(PropertyConstructorUInt32)
{
    MtpProperty *prop = new MtpProperty(MTP_PROPERTY_STORAGE_ID, MTP_TYPE_UINT32, true, 42);

    BOOST_CHECK (prop);
    BOOST_CHECK (prop->mType == MTP_TYPE_UINT32);
    BOOST_CHECK (prop->mCode == MTP_PROPERTY_STORAGE_ID);
    BOOST_CHECK (prop->mWriteable == true);

    BOOST_CHECK (prop->mDefaultValue.u.u32 == 42);
    BOOST_CHECK (prop->mCurrentValue.u.u64 == 0);
}

BOOST_AUTO_TEST_CASE(PropertySetFormRange)
{
    MtpProperty *prop = new MtpProperty(MTP_PROPERTY_STORAGE_ID, MTP_TYPE_UINT32, true, 42);

    prop->setFormRange(0, 90, 2);

    BOOST_CHECK(prop->mMinimumValue.u.u32 == 0);
    BOOST_CHECK(prop->mMaximumValue.u.u32 == 90);
    BOOST_CHECK(prop->mStepSize.u.u32 == 2);
}

BOOST_AUTO_TEST_CASE(PropertySetFormEnum)
{
    MtpProperty *prop = new MtpProperty(MTP_PROPERTY_STORAGE_ID, MTP_TYPE_UINT32, true, 42);
    const int values[4] = { 1, 2, 3, 4, };

    prop->setFormEnum(values, 4);

    BOOST_CHECK(prop->mEnumValues[0].u.u32 == 1);
    BOOST_CHECK(prop->mEnumValues[1].u.u32 == 2);
    BOOST_CHECK(prop->mEnumValues[2].u.u32 == 3);
    BOOST_CHECK(prop->mEnumValues[3].u.u32 == 4);
}

BOOST_AUTO_TEST_CASE(PropertySetFormDateTime)
{
    MtpProperty *prop = new MtpProperty(MTP_PROPERTY_STORAGE_ID, MTP_TYPE_UINT32, true, 42);

    prop->setFormDateTime();

    BOOST_CHECK(prop->mFormFlag == MtpProperty::kFormDateTime);
}

BOOST_AUTO_TEST_CASE(PropertyPrintToBuffer)
{
    MtpProperty *prop = new MtpProperty(MTP_PROPERTY_STORAGE_ID, MTP_TYPE_UINT32, true, 42);
    std::string expected ("42");
    std::string result;

    prop->print(prop->mDefaultValue, result);

    BOOST_CHECK (result == expected);
}
