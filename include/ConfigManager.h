//
// Created by nate on 12/25/25.
//

#ifndef RTL_SIGNALD_CONFIGMANAGER_H
#define RTL_SIGNALD_CONFIGMANAGER_H
#include <string>


class ConfigManager {
    std::string m_command {};
    std::string m_signal_code {};
    static void update_property(std::string_view property_name, std::string_view value);
public:
    explicit ConfigManager();
    std::string_view getCommand() { return m_command; }
    std::string_view getSignalCode() { return m_signal_code; }
    void setCommand(const std::string& command) {
        m_command = command;
        update_property("command", command);
    }
    void setSignalCode(const std::string& signal_code) {
        m_signal_code = signal_code;
        update_property("signal_code", signal_code);
    }
};


#endif //RTL_SIGNALD_CONFIGMANAGER_H