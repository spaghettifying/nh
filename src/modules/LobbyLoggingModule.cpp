#include "LobbyLoggingModule.h"
#include "../Config.h"
#include "../core/EventSystem.h"
#include "../CppPluginExample.h"
#include "../r2sdk/classes/CBaseEntity.h"
#include "../r2sdk/interfaces/IVEngineClient.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

LobbyLoggingModule::LobbyLoggingModule() {
    last_check_time_ = std::chrono::steady_clock::now();
}

void LobbyLoggingModule::Initialize() {
    std::cout << "LobbyLoggingModule: Initializing..." << std::endl;
    
    // Load configuration
    OnConfigChanged();
    
    // Subscribe to events
    auto& eventSystem = EventSystem::GetInstance();
    eventSystem.Subscribe(EventSystem::Events::PLAYER_CONNECTED, 
        [this](const std::string& /*eventId*/, void* data) {
            if (data) {
                std::string* playerInfo = static_cast<std::string*>(data);
                std::cout << "Player connected: " << *playerInfo << std::endl;
            }
        });
    
    std::cout << "LobbyLoggingModule: Initialized successfully" << std::endl;
}

void LobbyLoggingModule::Shutdown() {
    std::cout << "LobbyLoggingModule: Shutting down..." << std::endl;
    
    // Save any remaining logs
    SaveLogToFile();
    
    std::cout << "LobbyLoggingModule: Shutdown complete" << std::endl;
}

void LobbyLoggingModule::OnFrameUpdate() {
    if (!enabled_) return;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_check_time_).count();
    
    if (elapsed >= check_interval_seconds_) {
        CheckLobbyPlayers();
        last_check_time_ = now;
    }
}

void LobbyLoggingModule::OnClientLibraryLoaded() {
    std::cout << "LobbyLoggingModule: Client library loaded, ready for lobby monitoring" << std::endl;
}

void LobbyLoggingModule::OnConfigChanged() {
    extern Config g_config;
    
    enabled_ = g_config.isLobbyLoggingEnabled();
    check_interval_seconds_ = g_config.getLobbyLogInterval();
    log_file_path_ = g_config.getLobbyLogFile();
    
    std::cout << "LobbyLoggingModule: Configuration updated - " 
              << "Enabled: " << (enabled_ ? "Yes" : "No") 
              << ", Interval: " << check_interval_seconds_ << "s" << std::endl;
}

void LobbyLoggingModule::CheckLobbyPlayers() {
    // Check if we have access to the engine client interface
    if (!g_Interfaces.EngineClient) {
        return;
    }
    
    // Iterate through all possible player slots (Titanfall 2 supports up to 64 players)
    for (int i = 0; i < 64; ++i) {
        player_info_t playerInfo;
        
        // Try to get player info from the engine
        if (g_Interfaces.EngineClient->GetPlayerInfo(i, &playerInfo)) {
            // Extract UID (guid) and name
            std::string uid = std::string(playerInfo.guid);
            std::string name = std::string(playerInfo.name);
            
            // Skip if UID is empty or player is a bot
            if (uid.empty() || playerInfo.fakeplayer) {
                continue;
            }
            
            // Check if this is a new UID
            if (player_names_.find(uid) == player_names_.end()) {
                // New player, log their initial info
                LogPlayerInfo(uid, name);
                player_names_[uid] = {name};
                current_names_[uid] = name;
            } else {
                // Existing UID, check if name has changed
                std::string currentName = current_names_[uid];
                if (currentName != name) {
                    // Name has changed, log the change and update records
                    LogNameChange(uid, currentName, name);
                    
                    // Add new name to the list if it's not already there
                    auto& names = player_names_[uid];
                    if (std::find(names.begin(), names.end(), name) == names.end()) {
                        names.push_back(name);
                    }
                    
                    current_names_[uid] = name;
                }
            }
        }
    }
    
    if (player_names_.size() > 0) {
        std::cout << "LobbyLoggingModule: Checked for lobby players (found " 
                  << player_names_.size() << " unique players)" << std::endl;
    }
}

void LobbyLoggingModule::LogPlayerInfo(const std::string& uid, const std::string& name) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    
    std::cout << "LobbyLoggingModule: Logging new player - UID: " << uid << ", Name: " << name << std::endl;
    
    // Add to internal log (you might want to store this in a vector for batch writing)
    // For now, write directly to file
    std::ofstream logFile(log_file_path_, std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << ss.str() << "] NEW PLAYER - UID: " << uid << " | Name: " << name << std::endl;
        logFile.close();
    }
    
    // Publish event for other modules
    std::string playerInfo = name + " (" + uid + ")";
    EventSystem::GetInstance().Publish(EventSystem::Events::PLAYER_CONNECTED, &playerInfo);
}

void LobbyLoggingModule::LogNameChange(const std::string& uid, const std::string& oldName, const std::string& newName) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    
    std::cout << "LobbyLoggingModule: Name change detected - UID: " << uid 
              << ", Old Name: " << oldName << ", New Name: " << newName << std::endl;
    
    // Log the name change to file
    std::ofstream logFile(log_file_path_, std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << ss.str() << "] NAME CHANGE - UID: " << uid 
                << " | Old Name: " << oldName << " | New Name: " << newName;
        
        // Also show all known names for this UID
        auto& names = player_names_[uid];
        logFile << " | All Known Names: ";
        for (size_t i = 0; i < names.size(); ++i) {
            if (i > 0) logFile << ", ";
            logFile << names[i];
        }
        // Add the new name if it's not already in the list
        if (std::find(names.begin(), names.end(), newName) == names.end()) {
            logFile << ", " << newName;
        }
        logFile << std::endl;
        logFile.close();
    }
}

void LobbyLoggingModule::SaveLogToFile() {
    // This would save any buffered log entries
    // Implementation depends on your logging strategy
    std::cout << "LobbyLoggingModule: Saving logs to " << log_file_path_ << std::endl;
}
