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

#ifndef STUB_MOCK_MTP_DATABASE_H_
#define STUB_MOCK_MTP_DATABASE_H_

#include <mtp.h>
#include <MtpDatabase.h>
#include <MtpDataPacket.h>
#include <MtpObjectInfo.h>
#include <MtpProperty.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <tuple>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

namespace android
{
class MockMtpDatabase : public android::MtpDatabase {
private:
    struct DbEntry
    {
        MtpStorageID storage_id;
        std::string object_name;
        MtpObjectFormat object_format;
        MtpObjectHandle parent;
        size_t object_size;
        std::string display_name;
        std::string path;
    };

    uint32_t counter;
    std::map<MtpObjectHandle, DbEntry> db;
    
public:
    MockMtpDatabase() : counter(1)
    {
        DbEntry entry;
	db = std::map<MtpObjectHandle, DbEntry>();

        entry.storage_id = 666;
        entry.object_name = std::string("Test Object");
        entry.object_format = MTP_FORMAT_PNG;
        entry.parent = MTP_PARENT_ROOT;
        entry.object_size = 666;
        entry.display_name = std::string("Test Object");
        entry.path = std::string("TestObject");

        db.insert( std::pair<MtpObjectHandle, DbEntry>(counter, entry) );
    }

    virtual ~MockMtpDatabase() {}

    virtual void addStoragePath(const MtpString& path, const MtpString& displayName, MtpStorageID storage, bool hidden)
    {
    }

    virtual void removeStorage(MtpStorageID storage)
    {
    }

    virtual MtpObjectHandle beginSendObject(
        const MtpString& path,
        MtpObjectFormat format,
        MtpObjectHandle parent,
        MtpStorageID storage,
        uint64_t size,
        time_t modified)
    {
        return 1;
    }

    virtual void endSendObject(
        const MtpString& path,
        MtpObjectHandle handle,
        MtpObjectFormat format,
        bool succeeded)
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << path << std::endl;

	if (!succeeded) {
		db.erase(handle);
	}
    }

    virtual MtpObjectHandleList* getObjectList(
        MtpStorageID storageID,
        MtpObjectFormat format,
        MtpObjectHandle parent)
    {
        MtpObjectHandleList* list = nullptr;

        if (db.empty()) {
            list = new MtpObjectHandleList();
        } else {
            std::vector<MtpObjectHandle> keys;
            keys.push_back(1);
            list = new MtpObjectHandleList(keys);
        }
        
        return list;
    }

    virtual int getNumObjects(
        MtpStorageID storageID,
        MtpObjectFormat format,
        MtpObjectHandle parent)
    {
        return db.size();
    }

    virtual MtpObjectFormatList* getSupportedPlaybackFormats()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        static const MtpObjectFormatList list = {MTP_FORMAT_PNG};
        return new MtpObjectFormatList{list};
    }
    
    virtual MtpObjectFormatList* getSupportedCaptureFormats()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        static const MtpObjectFormatList list = {MTP_FORMAT_ASSOCIATION, MTP_FORMAT_PNG};
        return new MtpObjectFormatList{list};
    }
    
    virtual MtpObjectPropertyList* getSupportedObjectProperties(MtpObjectFormat format)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
            
        static const MtpObjectPropertyList list = 
        {
            MTP_PROPERTY_STORAGE_ID,
            MTP_PROPERTY_PARENT_OBJECT,
            MTP_PROPERTY_OBJECT_FORMAT,
            MTP_PROPERTY_OBJECT_SIZE,
            MTP_PROPERTY_WIDTH,
            MTP_PROPERTY_HEIGHT,
            MTP_PROPERTY_IMAGE_BIT_DEPTH,
            MTP_PROPERTY_DISPLAY_NAME            
        };
         
        return new MtpObjectPropertyList{list};
    }
    
    virtual MtpDevicePropertyList* getSupportedDeviceProperties()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        static const MtpDevicePropertyList list = { MTP_DEVICE_PROPERTY_UNDEFINED };
        return new MtpDevicePropertyList{list};
    }

    virtual MtpResponseCode getObjectPropertyValue(
        MtpObjectHandle handle,
        MtpObjectProperty property,
        MtpDataPacket& packet)
    {        
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        switch(property)
        {
            case MTP_PROPERTY_STORAGE_ID: packet.putUInt32(db.at(handle).storage_id); break;            
            case MTP_PROPERTY_PARENT_OBJECT: packet.putUInt32(db.at(handle).parent); break;            
            case MTP_PROPERTY_OBJECT_FORMAT: packet.putUInt32(db.at(handle).object_format); break;
            case MTP_PROPERTY_OBJECT_SIZE: packet.putUInt32(db.at(handle).object_size); break;
            case MTP_PROPERTY_DISPLAY_NAME: packet.putString(db.at(handle).display_name.c_str()); break;
            default: return MTP_RESPONSE_GENERAL_ERROR; break;                
        }
        
        return MTP_RESPONSE_OK;
    }

    virtual MtpResponseCode setObjectPropertyValue(
        MtpObjectHandle handle,
        MtpObjectProperty property,
        MtpDataPacket& packet)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
    }

    virtual MtpResponseCode getDevicePropertyValue(
        MtpDeviceProperty property,
        MtpDataPacket& packet)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_GENERAL_ERROR;
    }

    virtual MtpResponseCode setDevicePropertyValue(
        MtpDeviceProperty property,
        MtpDataPacket& packet)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
    }

    virtual MtpResponseCode resetDeviceProperty(
        MtpDeviceProperty property)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
    }

    virtual MtpResponseCode getObjectPropertyList(
        MtpObjectHandle handle,
        uint32_t format, 
        uint32_t property,
        int groupCode, 
        int depth,
        MtpDataPacket& packet)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
    }

    virtual MtpResponseCode getObjectInfo(
        MtpObjectHandle handle,
        MtpObjectInfo& info)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        info.mHandle = handle;
        info.mStorageID = db.at(handle).storage_id;
        info.mFormat = db.at(handle).object_format;
        info.mProtectionStatus = 0x0;
        info.mCompressedSize = 0;
        info.mThumbFormat = db.at(handle).object_format;
        info.mThumbCompressedSize = 20*20*4;
        info.mThumbPixWidth = 20;
        info.mThumbPixHeight  =20;
        info.mImagePixWidth = 20;
        info.mImagePixHeight = 20;
        info.mImagePixDepth = 4;
        info.mParent = db.at(handle).parent;
        info.mAssociationType = 0;
        info.mAssociationDesc = 0;
        info.mSequenceNumber = 0;
        info.mName = ::strdup(db.at(handle).object_name.c_str());
        info.mDateCreated = 0;
        info.mDateModified = 0;
        info.mKeywords = ::strdup("test");
        
        return MTP_RESPONSE_OK;
    }

    virtual void* getThumbnail(MtpObjectHandle handle, size_t& outThumbSize)
    {
        void* result;

	outThumbSize = 0;
	memset(result, 0, outThumbSize);

        return result;
    }

    virtual MtpResponseCode getObjectFilePath(
        MtpObjectHandle handle,
        MtpString& outFilePath,
        int64_t& outFileLength,
        MtpObjectFormat& outFormat)
    {
        DbEntry entry = db.at(handle);

        std::cout << __PRETTY_FUNCTION__ << std::endl;

        outFilePath = std::string(entry.path);
        outFileLength = entry.object_size;
        outFormat = entry.object_format;

        return MTP_RESPONSE_OK;
    }

    virtual MtpResponseCode deleteFile(MtpObjectHandle handle)
    {
        size_t orig_size = db.size();
        size_t new_size;

        std::cout << __PRETTY_FUNCTION__ << std::endl;

        new_size = db.erase(handle);

        if (orig_size > new_size) {
            BOOST_FOREACH(MtpObjectHandle i, db | boost::adaptors::map_keys) {
                if (db.at(i).parent == handle)
                    db.erase(i);
            }
            return MTP_RESPONSE_OK;
        }
        else
            return MTP_RESPONSE_GENERAL_ERROR;
    }

    virtual MtpResponseCode moveFile(MtpObjectHandle handle, MtpObjectHandle new_parent)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        db.at(handle).parent = new_parent;

        return MTP_RESPONSE_OK;
    }

    virtual MtpObjectHandleList* getObjectReferences(MtpObjectHandle handle)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return nullptr;
    }

    virtual MtpResponseCode setObjectReferences(
        MtpObjectHandle handle,
        MtpObjectHandleList* references)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;    
    }

    virtual MtpProperty* getObjectPropertyDesc(
        MtpObjectProperty property,
        MtpObjectFormat format)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        MtpProperty* result = nullptr;
        switch(property)
        {
            case MTP_PROPERTY_STORAGE_ID: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_OBJECT_FORMAT: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_OBJECT_SIZE: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_WIDTH: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_HEIGHT: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_IMAGE_BIT_DEPTH: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_DISPLAY_NAME: result = new MtpProperty(property, MTP_TYPE_STR); break;
            default: break;                
        }
        
        return result;
    }

    virtual MtpProperty* getDevicePropertyDesc(MtpDeviceProperty property)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return new MtpProperty(MTP_DEVICE_PROPERTY_UNDEFINED, MTP_TYPE_UNDEFINED);
    }
    
    virtual void sessionStarted(MtpServer* server)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    virtual void sessionEnded()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};
}

#endif // STUB_MOCK_MTP_DATABASE_H_
