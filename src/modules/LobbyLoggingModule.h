#pragma once

#include "../core/IFeatureModule.h"
#include <chrono>
#include <map>
#include <set>
#include <string>
#include <vector>

/**
 * Lobby Logging Module
 * Handles player UID logging and lobby information tracking
 */
class LobbyLoggingModule : public IFeatureModule {
public:
    LobbyLoggingModule();
    virtual ~LobbyLoggingModule() = default;
    
    // IFeatureModule interface
    void Initialize() override;
    void Shutdown() override;
    void OnFrameUpdate() override;
    void OnClientLibraryLoaded() override;
    void OnConfigChanged() override;
    
    const char* GetModuleName() const override { return "LobbyLogging"; }
    bool IsEnabled() const override { return enabled_; }
    void SetEnabled(bool enabled) override { enabled_ = enabled; }
    
private:
    void CheckLobbyPlayers();
    void LogPlayerInfo(const std::string& uid, const std::string& name);
    void LogNameChange(const std::string& uid, const std::string& oldName, const std::string& newName);
    void SaveLogToFile();
    
    bool enabled_ = true;
    std::chrono::steady_clock::time_point last_check_time_;
    std::map<std::string, std::vector<std::string>> player_names_; // UID -> list of names used
    std::map<std::string, std::string> current_names_; // UID -> current name
    std::string log_file_path_;
    int check_interval_seconds_ = 5;
};
