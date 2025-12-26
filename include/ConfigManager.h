//
// Created by nate on 12/25/25.
//

#ifndef RTL_SIGNALD_CONFIGMANAGER_H
#define RTL_SIGNALD_CONFIGMANAGER_H
#include <string>
#include <iostream>


class ConfigManager {
    std::string m_command {};
    std::string m_signal_code {};
    static void update_property(std::string_view property_name, std::string_view value);
public:
    explicit ConfigManager();
    [[nodiscard]] std::string_view getCommand() const { return m_command; }
    [[nodiscard]] std::string_view getSignalCode() const { return m_signal_code; }
    void setCommand(const std::string& command) {
        m_command = command;
        update_property("command", command);
    }
    void setSignalCode(const std::string& signal_code) {
        m_signal_code = signal_code;
        update_property("signal_code", signal_code);
    }
};

inline std::ostream& operator<<(std::ostream& os, const ConfigManager& config) {
   os << "Configuration loaded from config.properties:\n"
    << "  signal_code: " << config.getSignalCode().data() << "\n"
    << "  command: " << config.getCommand().data() << "\n";
    return os;
}


#endif //RTL_SIGNALD_CONFIGMANAGER_H