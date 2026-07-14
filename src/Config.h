#pragma once

#include "pch.h"
#include <unordered_map>
#include <string>

class Config {
private:
    std::unordered_map<std::string, bool> boolSettings;
    std::unordered_map<std::string, int> intSettings;
    std::unordered_map<std::string, float> floatSettings;
    std::unordered_map<std::string, std::string> stringSettings;
    std::string configFilePath;
    
    void setDefaults();
    void loadFromFile();
    void saveToFile();
    
public:
    Config();
    
    // Boolean settings
    bool getBool(const std::string& key);
    void setBool(const std::string& key, bool value);
    
    // Integer settings
    int getInt(const std::string& key);
    void setInt(const std::string& key, int value);
    
    // Float settings
    float getFloat(const std::string& key);
    void setFloat(const std::string &key, float value);

    // String settings
    std::string getString(const std::string& key);
    void setString(const std::string& key, const std::string& value);
    
    // Utility methods
    void reload();
    void save();
    void logCurrentSettings();
    
    // Feature toggles (convenience methods)
    bool isLobbyLoggingEnabled() { return getBool("lobby_logging_enabled"); }
    bool isNetVarDumpEnabled() { return getBool("netvar_dump_enabled"); }
    bool isDebugLoggingEnabled() { return getBool("debug_logging_enabled"); }
    bool isESPEnabled() { return getBool("esp_enabled"); }
    bool isRendererSelfTestEnabled() { return getBool("renderer_self_test_enabled"); }
    bool isDisplayModuleListEnabled() { return getBool("display_module_list"); }
    bool isDatamapDumperEnabled() { return getBool("datamap_dumper_enabled"); }

    int getLobbyLogInterval() { return getInt("lobby_log_interval"); }
    
    std::string getLobbyLogFile() { return getString("lobby_log_file"); }
};

// Global config instance
extern Config g_config;
