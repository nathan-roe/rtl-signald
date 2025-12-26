//
// Created by nate on 12/24/25.
//

#ifndef RTL_SIGNALD_SIGNALMONITOR_H
#define RTL_SIGNALD_SIGNALMONITOR_H
#include <chrono>
#include <functional>
#include <future>
#include <string_view>

#include "ConfigManager.h"
#include "DeviceMonitor.h"


class SignalMonitor {
    FILE *m_pipe {};
    ConfigManager *m_config {};
    DeviceMonitor *m_device_monitor {};
    std::chrono::steady_clock::time_point m_last_received_time;
    bool m_is_listening { };

    void define_signal_code_async(std::promise<const std::string_view&>& promise_obj);
    void start_listener(const std::function<bool(std::string code)>& callback);
public:
    explicit SignalMonitor(ConfigManager *config);
    ~SignalMonitor();

    void run();
};


#endif //RTL_SIGNALD_SIGNALMONITOR_H