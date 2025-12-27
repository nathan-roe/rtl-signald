//
// Created by Nathan Roe on 12/24/25.
//

#include "../include/SignalMonitor.h"

#include <iostream>
#include <cstdio>
#include <functional>
#include <future>

#include "ConfigManager.h"

#define RTL_COMMAND "rtl_433 -vA 2>/dev/null"
#define PIPE_MODE "r"
#define RTL_433_CODE_OUTPUT "codes     : "

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
            if (accept_or_decline == "y") {
                m_config->setSignalCode(received_code);
                promise_obj.set_value(received_code);
                return false;
            }
            return true; // continue listening for signals
        }
    };
    std::cout << "Starting signal listener\n";
    start_listener(callback);
}

void SignalMonitor::start_listener(const std::function<bool(std::string)>& callback) {
    if (!m_pipe) {
        std::cerr << "Failed to open rtl_433" << std::endl;
        exit(EXIT_FAILURE);
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
            if (!callback(code)) {
                break;
            }
        }
    }
    m_is_listening = false;
}

void SignalMonitor::run(const std::function<bool(std::string)>& callback) {
    std::promise<const std::string_view&> promise_obj {};
    if (m_config->getSignalCode().empty()) {
        std::cout << "No signal code is defined. Entering interactive mode\n";
        define_signal_code_async(promise_obj);
        promise_obj.get_future().wait();
    }
    start_listener(callback);
}
