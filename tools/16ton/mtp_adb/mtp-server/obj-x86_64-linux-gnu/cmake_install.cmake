# Install script for directory: /home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "None")
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

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
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
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/obj-x86_64-linux-gnu/libmtpserver.so.1.0.0"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/obj-x86_64-linux-gnu/libmtpserver.so.1"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/obj-x86_64-linux-gnu/libmtpserver.so"
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

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mtp" TYPE FILE FILES
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpDatabase.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpDataPacket.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpDebug.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpDevice.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpDeviceInfo.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpEventPacket.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/mtp.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpObjectInfo.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpPacket.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpProperty.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpRequestPacket.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpResponsePacket.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpServer.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpStorage.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpStorageInfo.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpStringBuffer.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpTypes.h"
    "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/include/MtpUtils.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/obj-x86_64-linux-gnu/libusbhost/cmake_install.cmake")
  include("/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/obj-x86_64-linux-gnu/server/cmake_install.cmake")
  include("/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/obj-x86_64-linux-gnu/tests/cmake_install.cmake")
  include("/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/obj-x86_64-linux-gnu/po/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/maddocks/kernels/linux-4.14.10-gpd/tools/16ton/mtp_adb/mtp-server/obj-x86_64-linux-gnu/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
