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
#include "MockMtpDatabase.h"
#include "MtpServer.h"
#include "MtpStorage.h"

using namespace android;

BOOST_AUTO_TEST_CASE(ServerConstructor)
{
    MtpDatabase *db = new MockMtpDatabase ();
    MtpServer *server = new MtpServer (0, db, false, 0, 0, 0);

    BOOST_CHECK(server);
}

BOOST_AUTO_TEST_CASE(ServerAddGetStorage)
{
    MtpDatabase *db = new MockMtpDatabase ();
    MtpServer *server = new MtpServer (0, db, false, 0, 0, 0);
    MtpStorage *storage = new MtpStorage (666, "/tmp", "Test storage", 0, false, 64);

    server->addStorage(storage);

    BOOST_CHECK(server->getStorage(666) != NULL);
}

BOOST_AUTO_TEST_CASE(ServerGetStorageNull)
{
    MtpDatabase *db = new MockMtpDatabase ();
    MtpServer *server = new MtpServer (0, db, false, 0, 0, 0);

    BOOST_CHECK(server->getStorage(666) == NULL);
}

BOOST_AUTO_TEST_CASE(ServerHasStorageTrue)
{
    MtpDatabase *db = new MockMtpDatabase ();
    MtpServer *server = new MtpServer (0, db, false, 0, 0, 0);
    MtpStorage *storage = new MtpStorage (666, "/tmp", "Test storage", 0, false, 64);

    server->addStorage(storage);

    BOOST_CHECK(server->hasStorage(666));
}

BOOST_AUTO_TEST_CASE(ServerHasStorageFalse)
{
    MtpDatabase *db = new MockMtpDatabase ();
    MtpServer *server = new MtpServer (0, db, false, 0, 0, 0);

    BOOST_CHECK(server->hasStorage(667) == false);
}
