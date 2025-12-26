//
// Created by nate on 12/24/25.
//

#ifndef RTL_SIGNALD_DEVICEMONITOR_H
#define RTL_SIGNALD_DEVICEMONITOR_H
#include <functional>
#include <libudev.h>
#include <optional>


class DeviceMonitor {
public:
    static void monitor(const std::optional<std::function<void(udev_device *dev)>>& on_connect,
                        const std::optional<std::function<void(udev_device *dev)>>& on_remove);
    static bool is_rtl_available();
};


#endif //RTL_SIGNALD_DEVICEMONITOR_H