//
// Created by Nathan Roe on 12/25/25.
//

#include "../include/ConfigManager.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <pwd.h>

#define PROPERTIES_FILE_PATH "/.config/rtlsignald.properties"

std::string ConfigManager::get_config_path() {
    const char* homedir = std::getenv("HOME");
    if (!homedir) {
        const uid_t uid = getuid(); // Get the effective user ID
        if (const passwd *pw = getpwuid(uid); pw != nullptr) {
            homedir = pw->pw_dir;
        }
    }
    return std::string(homedir) + PROPERTIES_FILE_PATH;
}

ConfigManager::ConfigManager() {
    // Load configuration from config.properties
    std::unordered_map<std::string, std::string> config;
    const std::string config_file_path = get_config_path();
    std::ifstream config_file(config_file_path);
    if (!config_file.is_open()) {
        std::cerr << "Warning: Could not open properties file at " << config_file_path << ". Using default/empty configuration.\n";
    } else {
        std::cout << "Using configuration file at: " << config_file_path << '\n';
    }
    if (config_file.is_open()) {
        std::string line;
        while (std::getline(config_file, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            if (std::string key, value; std::getline(iss, key, '=') && std::getline(iss, value)) {
                config[key] = value;
            }
        }
        config_file.close();
    }

    m_command = config["command"];
    m_signal_code = config["signal_code"];
    std::cout << *this;
}

void ConfigManager::update_property(std::string_view property_name, std::string_view value) {
    // Read all lines from file
    std::vector<std::string> lines;
    bool property_found = false;
    const std::string config_file_path = get_config_path();
    if (std::ifstream config_file(config_file_path); config_file.is_open()) {
        std::string line;
        while (std::getline(config_file, line)) {
            if (!line.empty() && line[0] != '#') {
                std::istringstream iss(line);
                std::string key;
                if (std::getline(iss, key, '=')) {
                    // Check if this is the property we want to update
                    if (key == property_name) {
                        // Replace this line with the updated property
                        lines.push_back(std::string(property_name) + "=" + std::string(value));
                        property_found = true;
                        continue;
                    }
                }
            }
            // Keep all other lines as-is (comments, empty lines, other properties)
            lines.push_back(line);
        }
        config_file.close();
    }

    // If property wasn't found, add it to the end
    if (!property_found) {
        lines.push_back(std::string(property_name) + "=" + std::string(value));
    }

    // Write all lines back to file
    std::cout << "Attempting to write to: " << config_file_path << '\n';
    if (std::ofstream out_file(config_file_path); out_file.is_open()) {
        for (const auto &line: lines) {
            out_file << line << "\n";
        }
        out_file.close();
    } else {
        std::cerr << "Error: Could not open properties file for writing.\n";
    }
}