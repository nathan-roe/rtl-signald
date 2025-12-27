//
// Created by Nathan Roe on 12/24/25.
//

#include "../include/DeviceMonitor.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <libudev.h>
#include <optional>
#include <string>
#include <sys/poll.h>

#define RT2838_PRODUCT_IDENTIFIER "RTL2838"

void DeviceMonitor::monitor(
    const std::optional<std::function<void(udev_device *dev)>>& on_connect,
    const std::optional<std::function<void(udev_device *dev)>>& on_remove)
{
    udev *udev = udev_new();
    if (!udev) {
        fprintf(stderr, "Can't create udev\n");
        exit(EXIT_FAILURE);
    }

    udev_monitor *mon = udev_monitor_new_from_netlink(udev, "udev");
    if (!mon) {
        fprintf(stderr, "Can't create udev monitor\n");
        udev_unref(udev);
        exit(EXIT_FAILURE);
    }

    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", nullptr);
    udev_monitor_enable_receiving(mon);

    const int fd = udev_monitor_get_fd(mon);
    while (true) {
        pollfd fds[1];
        fds[0].fd = fd;
        fds[0].events = POLLIN;

        if (const int ret = poll(fds, 1, -1); ret < 0) {
            fprintf(stderr, "Poll error: %s\n", strerror(errno));
            break;
        }

        if (fds[0].revents & POLLIN) {
            if (udev_device *dev = udev_monitor_receive_device(mon)) {
                const char* action_raw = udev_device_get_action(dev);
                if (const std::string_view action = action_raw ? action_raw : ""; action == "add" && on_connect.has_value()) {
                    on_connect.value()(dev);
                } else if (action == "remove" && on_remove.has_value()) {
                    on_remove.value()(dev);
                }
                udev_device_unref(dev);
            }
        }
    }

    udev_monitor_unref(mon);
    udev_unref(udev);
}

bool DeviceMonitor::is_rtl_available() {
    udev_list_entry *dev_list_entry;

    udev *udev = udev_new();
    if (!udev) {
        fprintf(stderr, "Unable to create udev while attempting to retrieve RTL device\n");
        return false;
    }

    udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "usb");
    udev_enumerate_scan_devices(enumerate);
    udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char* path = udev_list_entry_get_name(dev_list_entry);
        udev_device* dev = udev_device_new_from_syspath(udev, path);
        if (const char* product_name = udev_device_get_sysattr_value(dev, "product");
            product_name && std::string(product_name).starts_with(RT2838_PRODUCT_IDENTIFIER))
        {
            printf("RTL device found: %s\n", product_name);
            return true;
        }
        udev_device_unref(dev);
    }
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
    fprintf(stderr, "No RTL device was found\n");
    return false;
}
