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

#include "UbuntuMtpDatabase.h"

#include <MtpServer.h>
#include <MtpStorage.h>

#include <iostream>
#include <stdint.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <libintl.h>
#include <locale.h>

//#include <hybris/properties/properties.h>
#include <glog/logging.h>
/*
#include <core/dbus/bus.h>
#include <core/dbus/object.h>
#include <core/dbus/property.h>
#include <core/dbus/service.h>
#include <core/dbus/signal.h>

#include <core/dbus/asio/executor.h>
#include <core/dbus/types/stl/tuple.h>
#include <core/dbus/types/stl/vector.h>
#include <core/dbus/types/struct.h>
*/

//namespace dbus = core::dbus;
using namespace android;
/*
namespace core
{
dbus::Bus::Ptr the_session_bus()
{
    static dbus::Bus::Ptr session_bus = std::make_shared<dbus::Bus>(dbus::WellKnownBus::session);
    return session_bus;
}

struct UnityGreeter
{
    struct Properties
    {
        struct IsActive
        {
            inline static std::string name()
            {
                return "IsActive";
            };
            typedef UnityGreeter Interface;
            typedef bool ValueType;
            static const bool readable = true;
            static const bool writable = false;
        };
    };
};
}

namespace core
{
namespace dbus
{
namespace traits
{
template<>
struct Service<core::UnityGreeter>
{
    inline static const std::string& interface_name()
    {
        static const std::string s
        {
            "com.canonical.UnityGreeter"
        };
        return s;
    }
};
}
}
}
*/
namespace
{
struct FileSystemConfig
{
    static const int file_perm = 0664;
    static const int directory_perm = 0755;
};

}

class MtpDaemon
{

private:
    struct passwd *userdata;
//    dbus::Bus::Ptr bus;
    boost::thread dbus_thread;

    // Mtp stuff
    MtpServer* server;
    MtpStorage* home_storage;
    MtpStorage* sd_card;
    MtpDatabase* mtp_database;

    // Security
//    std::shared_ptr<core::dbus::Property<core::UnityGreeter::Properties::IsActive> > is_active;
//    bool screen_locked = true;
    bool screen_locked = false;

    // inotify stuff
    boost::thread notifier_thread;
    boost::thread io_service_thread;

    asio::io_service io_svc;
    asio::io_service::work work;
    asio::posix::stream_descriptor stream_desc;
    asio::streambuf buf;

    int inotify_fd;

    int watch_fd;
    int media_fd;

    // storage
    std::map<std::string, std::tuple<MtpStorage*, bool> > removables;
    bool home_storage_added;

    void add_removable_storage(const char *path, const char *name)
    {
        static int storageID = MTP_STORAGE_REMOVABLE_RAM;

        /* TODO check removable file system type to set maximum file size */
        MtpStorage *removable = new MtpStorage(
            storageID,
            path,
            name,
            1024 * 1024 * 100,  /* 100 MB reserved space, to avoid filling the disk */
            true,
            UINT64_C(4294967295)  /* 4GB-1, we assume vfat here */);

        storageID++;

        if (!screen_locked) {
            mtp_database->addStoragePath(path,
                                         std::string(),
                                         removable->getStorageID(),
                                         true);
            server->addStorage(removable);
        }

        removables.insert(std::pair<std::string, std::tuple<MtpStorage*, bool> >
                              (name,
                               std::make_tuple(removable,
                                               screen_locked ? false : true)));
    }

    void add_mountpoint_watch(const std::string& path)
    {
        VLOG(1) << "Adding notify watch for " << path;
        watch_fd = inotify_add_watch(inotify_fd,
                                     path.c_str(),
                                     IN_CREATE | IN_DELETE);
    }

    void read_more_notify()
    {
        VLOG(1) << __PRETTY_FUNCTION__;

        stream_desc.async_read_some(buf.prepare(buf.max_size()),
                                    boost::bind(&MtpDaemon::inotify_handler,
                                                this,
                                                asio::placeholders::error,
                                                asio::placeholders::bytes_transferred));
    }

    void inotify_handler(const boost::system::error_code&,
                         std::size_t transferred)
    {
        size_t processed = 0;

        while(transferred - processed >= sizeof(inotify_event))
        {
            const char* cdata = processed + asio::buffer_cast<const char*>(buf.data());
            const inotify_event* ievent = reinterpret_cast<const inotify_event*>(cdata);
            path storage_path ("/media");

            processed += sizeof(inotify_event) + ievent->len;

            storage_path /= userdata->pw_name;

            if (ievent->len > 0 && ievent->mask & IN_CREATE)
            {
                if (ievent->wd == media_fd) {
                    VLOG(1) << "media root was created for user " << ievent->name;
                    add_mountpoint_watch(storage_path.string());
                } else {
                    VLOG(1) << "Storage was added: " << ievent->name;
                    storage_path /= ievent->name;
                    add_removable_storage(storage_path.string().c_str(), ievent->name);
                }
            }
            else if (ievent->len > 0 && ievent->mask & IN_DELETE)
            {
                VLOG(1) << "Storage was removed: " << ievent->name;

                // Try to match to which storage was removed.
                BOOST_FOREACH(std::string name, removables | boost::adaptors::map_keys) {
                    if (name == ievent->name) {
                        auto t = removables.at(name);
                        MtpStorage *storage = std::get<0>(t);

                        VLOG(2) << "removing storage id "
                                << storage->getStorageID();

                        server->removeStorage(storage);
                        mtp_database->removeStorage(storage->getStorageID());
                    }
                }
            }
        }

        read_more_notify();
    }
/*
    void drive_bus()
    {
        try {
            bus->run();
        }
        catch (...) {
            PLOG(ERROR) << "There was an unexpected error in DBus; terminating.";
            server->stop();
        }
    }
*/
public:

    MtpDaemon(int fd):
        stream_desc(io_svc),
        work(io_svc),
        buf(1024)
    {
        userdata = getpwuid (getuid());

        // Removable storage hacks
        inotify_fd = inotify_init();
        if (inotify_fd <= 0)
            PLOG(FATAL) << "Unable to initialize inotify";
        VLOG(1) << "using inotify fd " << inotify_fd << " for daemon";

        stream_desc.assign(inotify_fd);
        notifier_thread = boost::thread(&MtpDaemon::read_more_notify, this);
        io_service_thread = boost::thread(boost::bind(&asio::io_service::run, &io_svc));


        // MTP database.
        mtp_database = new UbuntuMtpDatabase();


        // MTP server
        server = new MtpServer(
                fd,
                mtp_database,
                false,
                userdata->pw_gid,
                FileSystemConfig::file_perm,
                FileSystemConfig::directory_perm);

        // security / screen locking
//        bus = core::the_session_bus();
//       bus->install_executor(core::dbus::asio::make_executor(bus));
//        dbus_thread = boost::thread(&MtpDaemon::drive_bus, this);
//        auto greeter_service = dbus::Service::use_service(bus, "com.canonical.UnityGreeter");
//        dbus::Object::Ptr greeter = greeter_service->object_for_path(dbus::types::ObjectPath("/"));

//        is_active = greeter->get_property<core::UnityGreeter::Properties::IsActive>();
    }

    void initStorage()
    {
//        char product_name[PROP_VALUE_MAX];
        char product_name[16];

        // Local storage
//        property_get ("ro.product.model", product_name, "Ubuntu Touch device");

        home_storage = new MtpStorage(
            MTP_STORAGE_FIXED_RAM,
            userdata->pw_dir,
	    "MTP gadget", //product_name,
            1024 * 1024 * 100,  /* 100 MB reserved space, to avoid filling the disk */
            false,
            0  /* Do not check sizes for internal storage */);
/*
        mtp_database->addStoragePath(std::string(userdata->pw_dir) + "/Documents",
                                     gettext("Documents"),
                                     MTP_STORAGE_FIXED_RAM, false);
        mtp_database->addStoragePath(std::string(userdata->pw_dir) + "/Music",
                                     gettext("Music"),
                                     MTP_STORAGE_FIXED_RAM, false);
        mtp_database->addStoragePath(std::string(userdata->pw_dir) + "/Videos",
                                     gettext("Videos"),
                                     MTP_STORAGE_FIXED_RAM, false);
        mtp_database->addStoragePath(std::string(userdata->pw_dir) + "/Pictures",
                                     gettext("Pictures"),
                                     MTP_STORAGE_FIXED_RAM, false);
        mtp_database->addStoragePath(std::string(userdata->pw_dir) + "/Downloads",
                                     gettext("Downloads"),
                                     MTP_STORAGE_FIXED_RAM, false);
*/
        mtp_database->addStoragePath(std::string(userdata->pw_dir) + "/",
                                     gettext("root"),
                                     MTP_STORAGE_FIXED_RAM, false);
        home_storage_added = false;

        // Get any already-mounted removable storage.
        path p(std::string("/mnt/") + userdata->pw_name);
        if (exists(p)) {
            std::vector<path> v;
            copy(directory_iterator(p), directory_iterator(), std::back_inserter(v));
            for (std::vector<path>::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it)
            {
                add_removable_storage(it->string().c_str(), it->leaf().c_str());
            }

            // make sure we can catch any new removable storage that gets added.
            add_mountpoint_watch(p.string());
        } else {
            media_fd = inotify_add_watch(inotify_fd,
                                         "/media",
                                         IN_CREATE | IN_DELETE);
        }

    }

    ~MtpDaemon()
    {
        // Cleanup
        inotify_rm_watch(inotify_fd, watch_fd);
        io_svc.stop();
        dbus_thread.detach();
        notifier_thread.detach();
        io_service_thread.join();
        close(inotify_fd);
    }

    void run()
    {
/*
        if (is_active->get()) {
            is_active->changed().connect([this](bool active)
            {
                if (!active) {
                    screen_locked = active;
                    VLOG(2) << "device was unlocked, adding storage";
                    if (home_storage && !home_storage_added) {
                        server->addStorage(home_storage);
                        home_storage_added = true;
                    }
                    BOOST_FOREACH(std::string name, removables | boost::adaptors::map_keys) {
                        auto t = removables.at(name);
                        MtpStorage *storage = std::get<0>(t);
                        bool added = std::get<1>(t);
                        if (!added) {
                            mtp_database->addStoragePath(storage->getPath(),
                                                         std::string(),
                                                         storage->getStorageID(),
                                                         true);
                            server->addStorage(storage);
                        }
                    }
                }
            });
        } else {
*/
            screen_locked = false;
            VLOG(2) << "device is not locked, adding storage";
            if (home_storage) {
                server->addStorage(home_storage);
                home_storage_added = true;
            }
            BOOST_FOREACH(std::string name, removables | boost::adaptors::map_keys) {
                auto t = removables.at(name);
                MtpStorage *storage = std::get<0>(t);
                bool added = std::get<1>(t);
                if (!added) {
                    mtp_database->addStoragePath(storage->getPath(),
                                                 std::string(),
                                                 storage->getStorageID(),
                                                 true);
                    server->addStorage(storage);
                }
            }
 //       }

        // start the MtpServer main loop
        server->run();
    }
};

int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);

    bindtextdomain("mtp-server", "/usr/share/locale");
    setlocale(LC_ALL, "");
    textdomain("mtp-server");

    LOG(INFO) << "MTP server starting...";

    int fd = open("/dev/mtp_usb", O_RDWR);
    if (fd < 0)
    {
        LOG(ERROR) << "Error opening /dev/mtp_usb, aborting now...";
        return 1;
    }

    try {
        MtpDaemon *d = new MtpDaemon(fd);

        d->initStorage();
        d->run();

        delete d;
    }
    catch (std::exception& e) {
        /* If the daemon fails to initialize, ignore the error but
         * make sure to propagate the message and return with an
         * error return code.
         */
        LOG(ERROR) << "Could not start the MTP server:" << e.what();
    }
}
