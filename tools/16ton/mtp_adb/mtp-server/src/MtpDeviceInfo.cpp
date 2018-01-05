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

#define LOG_TAG "MtpDeviceInfo"

#include "MtpDebug.h"
#include "MtpDataPacket.h"
#include "MtpDeviceInfo.h"
#include "MtpStringBuffer.h"

#include <cstring>

#include <glog/logging.h>

namespace android {

MtpDeviceInfo::MtpDeviceInfo()
    :   mStandardVersion(0),
        mVendorExtensionID(0),
        mVendorExtensionVersion(0),
        mVendorExtensionDesc(NULL),
        mFunctionalMode(0),
        mOperations(NULL),
        mEvents(NULL),
        mDeviceProperties(NULL),
        mCaptureFormats(NULL),
        mPlaybackFormats(NULL),
        mManufacturer(NULL),
        mModel(NULL),
        mVersion(NULL),
        mSerial(NULL)
{
}

MtpDeviceInfo::~MtpDeviceInfo() {
    if (mVendorExtensionDesc)
        free(mVendorExtensionDesc);
    delete mOperations;
    delete mEvents;
    delete mDeviceProperties;
    delete mCaptureFormats;
    delete mPlaybackFormats;
    if (mManufacturer)
        free(mManufacturer);
    if (mModel)
        free(mModel);
    if (mVersion)
        free(mVersion);
    if (mSerial)
        free(mSerial);
}

bool MtpDeviceInfo::read(MtpDataPacket& packet) {
    MtpStringBuffer string;

    // read the device info
    if (!packet.getUInt16(mStandardVersion)) return false;
    if (!packet.getUInt32(mVendorExtensionID)) return false;
    if (!packet.getUInt16(mVendorExtensionVersion)) return false;

    if (!packet.getString(string)) return false;
    mVendorExtensionDesc = strdup((const char *)string);

    if (!packet.getUInt16(mFunctionalMode)) return false;
    mOperations = packet.getAUInt16();
    if (!mOperations) return false;
    mEvents = packet.getAUInt16();
    if (!mEvents) return false;
    mDeviceProperties = packet.getAUInt16();
    if (!mDeviceProperties) return false;
    mCaptureFormats = packet.getAUInt16();
    if (!mCaptureFormats) return false;
    mPlaybackFormats = packet.getAUInt16();
    if (!mCaptureFormats) return false;

    if (!packet.getString(string)) return false;
    mManufacturer = strdup((const char *)string);
    if (!packet.getString(string)) return false;
    mModel = strdup((const char *)string);
    if (!packet.getString(string)) return false;
    mVersion = strdup((const char *)string);
    if (!packet.getString(string)) return false;
    mSerial = strdup((const char *)string);

    return true;
}

void MtpDeviceInfo::print() {
    VLOG(2) << "Device Info:"
            << "\n\tmStandardVersion: " << mStandardVersion
            << "\n\tmVendorExtensionID: " << mVendorExtensionID
            << "\n\tmVendorExtensionVersion: " << mVendorExtensionVersion
            << "\n\tmVendorExtensionDesc: " << mVendorExtensionDesc
            << "\n\tmFunctionalCode: " << mFunctionalMode
            << "\n\tmManufacturer: " << mManufacturer
            << "\n\tmModel: " << mModel
            << "\n\tmVersion: " << mVersion
            << "\n\tmSerial: " << mSerial;
}

}  // namespace android
