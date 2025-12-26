#include "../include/SignalMonitor.h"

#include <iostream>
#include <cstdio>
#include <functional>
#include <future>
#include <thread>

#include "ConfigManager.h"
#include "DeviceMonitor.h"

#define RTL_COMMAND "rtl_433 -vA 2>/dev/null"
#define PIPE_MODE "r"
#define RTL_433_CODE_OUTPUT "codes     : "
#define DEBOUNCE_RATE_MS 1500

using std::chrono::steady_clock;

SignalMonitor::SignalMonitor(ConfigManager *config) : m_pipe{popen(RTL_COMMAND, PIPE_MODE)}, m_config { config } {
}

SignalMonitor::~SignalMonitor() {
    pclose(m_pipe);
}

void SignalMonitor::define_signal_code_async(std::promise<const std::string_view&>& promise_obj) {
    const std::function<bool(std::string received_code)> callback {
        [this, &promise_obj](const std::string& received_code) {
            std::string accept_or_decline {};
            std::cout << "Use [" << received_code <<"] for signal code? [y/n] ";
            std::cin >> std::ws >> accept_or_decline;
            std::transform(accept_or_decline.begin(), accept_or_decline.end(), accept_or_decline.begin(),
               [](const unsigned char c) {
                   return std::tolower(c);
               });
            m_last_received_time = steady_clock::now();
            if (accept_or_decline == "y") {
                m_config->setSignalCode(received_code);
                promise_obj.set_value(received_code);
                return false;
            }
            return true; // continue listening for signals
        }
    };
    start_listener(callback);
}

void SignalMonitor::start_listener(const std::function<bool(std::string)>& callback) {
    if (!m_pipe) {
        std::cerr << "Failed to open rtl_433" << std::endl;
        return;
    }
    char buffer[1024];
    m_is_listening = true;
    while (fgets(buffer, sizeof(buffer), m_pipe) != nullptr) {
        std::string line(buffer);
        std::string_view s {RTL_433_CODE_OUTPUT};
        if (const size_t pos = line.find(s); pos != std::string::npos) {
            std::string code = line.substr(pos + s.size());
            if (const size_t end = code.find_first_of("\r\n"); end != std::string::npos) {
                code = code.substr(0, end);
            }
            if(std::chrono::time_point<steady_clock> current_time = steady_clock::now();
                duration_cast<std::chrono::milliseconds>(current_time - m_last_received_time).count() > DEBOUNCE_RATE_MS
                && (m_config->getSignalCode().empty() || code == m_config->getSignalCode()))
            {
                m_last_received_time = current_time;
                if (!callback(code)) {
                    break;
                }
            }
        }
    }
    m_is_listening = false;
}

void SignalMonitor::run() {
    std::promise<const std::string_view&> promise_obj {};
    if (m_config->getSignalCode().empty()) {
        define_signal_code_async(promise_obj);
        promise_obj.get_future().wait();
    }
    const std::function<bool(std::string)> callback {
        [this](const std::string&) {
            const std::string_view system_command = m_config->getCommand();
            if (!system_command.empty()) {
                std::system(system_command.data());
            }
            return true;
        }
    };
    std::thread monitor_thread([this, callback] {
        DeviceMonitor::monitor([this, callback](const char *) {
                                      if (!m_is_listening) {
                                          start_listener(callback);
                                      }
                                  }, [this, callback](const char *) {
                                      callback(std::string(m_config->getSignalCode()));
                                  });
    });
    monitor_thread.detach();
    start_listener(callback);
}
