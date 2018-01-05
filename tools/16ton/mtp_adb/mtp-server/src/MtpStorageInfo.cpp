/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "MtpStorageInfo"

#include <iomanip>
#include <cstring>

#include "MtpDebug.h"
#include "MtpDataPacket.h"
#include "MtpStorageInfo.h"
#include "MtpStringBuffer.h"

#include <glog/logging.h>

namespace android {

MtpStorageInfo::MtpStorageInfo(MtpStorageID id)
    :   mStorageID(id),
        mStorageType(0),
        mFileSystemType(0),
        mAccessCapability(0),
        mMaxCapacity(0),
        mFreeSpaceBytes(0),
        mFreeSpaceObjects(0),
        mStorageDescription(NULL),
        mVolumeIdentifier(NULL)
{
}

MtpStorageInfo::~MtpStorageInfo() {
    if (mStorageDescription)
        free(mStorageDescription);
    if (mVolumeIdentifier)
        free(mVolumeIdentifier);
}

bool MtpStorageInfo::read(MtpDataPacket& packet) {
    MtpStringBuffer string;

    // read the device info
    if (!packet.getUInt16(mStorageType)) return false;
    if (!packet.getUInt16(mFileSystemType)) return false;
    if (!packet.getUInt16(mAccessCapability)) return false;
    if (!packet.getUInt64(mMaxCapacity)) return false;
    if (!packet.getUInt64(mFreeSpaceBytes)) return false;
    if (!packet.getUInt32(mFreeSpaceObjects)) return false;

    if (!packet.getString(string)) return false;
    mStorageDescription = strdup((const char *)string);
    if (!packet.getString(string)) return false;
    mVolumeIdentifier = strdup((const char *)string);

    return true;
}

void MtpStorageInfo::print() {
    VLOG(2) << "Storage Info " << std::hex << mStorageID << std::dec << ":"
            << "\n\tmStorageType: " << mStorageType
            << "\n\tmFileSystemType: " << mFileSystemType
            << "\n\tmAccessCapability: " << mAccessCapability;
    VLOG(2) << "\tmMaxCapacity: " << mMaxCapacity
            << "\n\tmFreeSpaceBytes: " << mFreeSpaceBytes
            << "\n\tmFreeSpaceObjects: " << mFreeSpaceObjects;
    VLOG(2) << "\tmStorageDescription: " << mStorageDescription
            << "\n\tmVolumeIdentifier: " << mVolumeIdentifier;
}

}  // namespace android
