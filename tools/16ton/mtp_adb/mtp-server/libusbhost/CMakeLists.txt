project(usbhost)

cmake_minimum_required(VERSION 2.8)

include_directories(include/)

add_library(
    usbhost
    src/usbhost.c
)

install(
  FILES libusbhost.a DESTINATION lib
)
