//
// Created by nate on 12/24/25.
//

#ifndef RTL_SIGNALD_DEVICEMONITOR_H
#define RTL_SIGNALD_DEVICEMONITOR_H
#include <functional>
#include <optional>


class DeviceMonitor {
public:
    static void monitor(const std::optional<std::function<void(const char *dev_node)>>& on_connect,
                        const std::optional<std::function<void(const char *dev_node)>>& on_remove);
};


#endif //RTL_SIGNALD_DEVICEMONITOR_H