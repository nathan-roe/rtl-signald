#include <cstring>

#include "SignalMonitor.h"

#define DEBOUNCE_RATE_MS 1500
#define DEFAULT_CALLBACK_MESSAGE "Event captured. Running configured command"

using std::chrono::steady_clock;

std::optional<std::string> parse_arguments(const int argc, char *argv[], const ConfigManager& config) {
    std::string command {};
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--command") == 0) {
            if (i + 1 < argc) {
                command = argv[i+1];
                i++;
            } else {
                std::cerr << "Error: --command argument requires a value." << std::endl;
                return std::nullopt;
            }
        } else if (strcmp(argv[i], "--require-code") == 0) {
            if (config.getSignalCode().empty()) {
                std::cerr << "The signal code must be defined in the configuration file, per provided command arguments.";
                exit(EXIT_FAILURE);
            }
        } else {
            std::cerr << "Warning: Unknown argument '" << argv[i] << "'" << std::endl;
        }
    }
    return command;
}

int main(const int argc, char *argv[]) {
    std::cout << "Application started\n";
    ConfigManager c {};
    if(const std::optional command { parse_arguments(argc, argv, c) }; command.has_value()) {
        c.setCommand(command.value());
    }

    [[maybe_unused]] steady_clock::time_point last_executed = steady_clock::now();
    auto callback = [&c, &last_executed](const std::string& code, const std::string_view message=DEFAULT_CALLBACK_MESSAGE) {
        if(const std::chrono::time_point<steady_clock> current_time = steady_clock::now();
                duration_cast<std::chrono::milliseconds>(current_time - last_executed).count() > DEBOUNCE_RATE_MS
                && (c.getSignalCode().empty() || code == c.getSignalCode())) {
            last_executed = current_time;
            const std::string_view system_command = c.getCommand();
            std::cout << '[' << code << "]: " << message << '\n';
            if (!system_command.empty()) {
                std::system(system_command.data());
            }
        }
        return true;
    };

    SignalMonitor sm { &c };
    if (!DeviceMonitor::is_rtl_available()) {
        return EXIT_FAILURE;
    }

    std::thread monitor_t { [&c, &callback] {
        const auto monitor_callback {[&c, &callback](udev_device *dev) {
            std::string monitor_message { DEFAULT_CALLBACK_MESSAGE };
            if (const char* prod_name = udev_device_get_sysattr_value(dev, "product")) {
                monitor_message = "Event triggered for device: " + std::string(prod_name);
            }
            callback(c.getSignalCode().data(), monitor_message);
        }};
        DeviceMonitor::monitor(monitor_callback, monitor_callback);
    } };
    monitor_t.detach();

    sm.run(callback);
    if (!DeviceMonitor::is_rtl_available()) {
        callback(c.getSignalCode().data());
    }

    return EXIT_SUCCESS;
}