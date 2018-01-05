# Install script for directory: /sdcard/mtp-0.0.4+16.04.20160413

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmtpserver.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmtpserver.so.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmtpserver.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/sdcard/mtp-0.0.4+16.04.20160413/build/libmtpserver.so.1.0.0"
    "/sdcard/mtp-0.0.4+16.04.20160413/build/libmtpserver.so.1"
    "/sdcard/mtp-0.0.4+16.04.20160413/build/libmtpserver.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmtpserver.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmtpserver.so.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmtpserver.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mtp" TYPE FILE FILES
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpDatabase.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpDataPacket.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpDebug.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpDevice.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpDeviceInfo.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpEventPacket.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/mtp.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpObjectInfo.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpPacket.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpProperty.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpRequestPacket.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpResponsePacket.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpServer.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpStorage.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpStorageInfo.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpStringBuffer.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpTypes.h"
    "/sdcard/mtp-0.0.4+16.04.20160413/include/MtpUtils.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/sdcard/mtp-0.0.4+16.04.20160413/build/libusbhost/cmake_install.cmake")
  include("/sdcard/mtp-0.0.4+16.04.20160413/build/server/cmake_install.cmake")
  include("/sdcard/mtp-0.0.4+16.04.20160413/build/tests/cmake_install.cmake")
  include("/sdcard/mtp-0.0.4+16.04.20160413/build/po/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/sdcard/mtp-0.0.4+16.04.20160413/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
