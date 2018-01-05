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
#include "MtpStorage.h"

using namespace android;

BOOST_AUTO_TEST_CASE(StorageConstructor)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, false, 64);

    BOOST_CHECK(s);
}

BOOST_AUTO_TEST_CASE(StorageGetTypeFixed)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, false, 64);

    BOOST_CHECK (s->getType() == MTP_STORAGE_FIXED_RAM);
}

BOOST_AUTO_TEST_CASE(StorageGetTypeRemovable)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getType() == MTP_STORAGE_REMOVABLE_RAM);
}

BOOST_AUTO_TEST_CASE(StorageGetFileSystemType)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getFileSystemType() == MTP_STORAGE_FILESYSTEM_HIERARCHICAL);
}

BOOST_AUTO_TEST_CASE(StorageGetAccessCapa)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getAccessCapability() == MTP_STORAGE_READ_WRITE);
}

BOOST_AUTO_TEST_CASE(StorageGetMaxCapacity)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getMaxCapacity() > 0);
}

BOOST_AUTO_TEST_CASE(StorageGetMaxCapacityInvalidPath)
{
    MtpStorage *s = new MtpStorage (666, "", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getMaxCapacity() == -1);
}

BOOST_AUTO_TEST_CASE(StoageGetFreeSpace)
{
    MtpStorage *s = new MtpStorage (666, "/", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getFreeSpace() != -1);
}

BOOST_AUTO_TEST_CASE(StorageGetDescription)
{
    MtpStorage *s = new MtpStorage (666, "/", "Test storage", 0, true, 64);

    BOOST_CHECK_EQUAL( strcmp( s->getDescription(), "Test storage" ), 0);
}
