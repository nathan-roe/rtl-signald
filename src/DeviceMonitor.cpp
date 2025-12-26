//
// Created by nate on 12/24/25.
//

#include "../include/DeviceMonitor.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <libudev.h>
#include <optional>
#include <string>
#include <sys/poll.h>

void DeviceMonitor::monitor(
    const std::optional<std::function<void(const char *dev_node)>>& on_connect,
    const std::optional<std::function<void(const char *dev_node)>>& on_remove)
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
                const char* dev_node = udev_device_get_devnode(dev);

                if (const std::string_view action = action_raw ? action_raw : ""; action == "add" && on_connect.has_value()) {
                    on_connect.value()(dev_node);
                } else if (action == "remove" && on_remove.has_value()) {
                    on_remove.value()(dev_node);
                }
                udev_device_unref(dev);
            }
        }
    }

    udev_monitor_unref(mon);
    udev_unref(udev);
}
