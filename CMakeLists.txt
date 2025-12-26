cmake_minimum_required(VERSION 4.1)
project(rtl-signald)

set(CMAKE_CXX_STANDARD 26)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(LIBUDEV REQUIRED libudev)
pkg_check_modules(LIBRTLSDR REQUIRED librtlsdr)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

add_executable(rtl-signald src/main.cpp
        src/DeviceMonitor.cpp
        include/DeviceMonitor.h
        src/SignalMonitor.cpp
        include/SignalMonitor.h
        src/ConfigManager.cpp
        include/ConfigManager.h
)

target_include_directories(rtl-signald PRIVATE
        include
        ${LIBUDEV_INCLUDE_DIRS}
        ${LIBRTLSDR_INCLUDE_DIRS}
        ${LIBUSB_INCLUDE_DIRS}
)

target_link_libraries(rtl-signald
        PRIVATE
        ${LIBUDEV_LIBRARIES}
        ${LIBRTLSDR_LIBRARIES}
        ${LIBUSB_LIBRARIES}
        Threads::Threads
)