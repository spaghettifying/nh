#pragma once

#include "../core/IFeatureModule.h"
#include "../core/EventSystem.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

/**
 * Enhanced Plugin Callbacks Manager
 * Manages feature modules and provides better organization
 */
class PluginCallbacksManager {
public:
    static PluginCallbacksManager& GetInstance();
    
    // Module management
    void RegisterModule(std::unique_ptr<IFeatureModule> module);
    void UnregisterModule(const std::string& moduleName);
    IFeatureModule* GetModule(const std::string& moduleName);
    
    // Lifecycle management
    void Initialize();
    void Shutdown();
    
    // Event forwarding to modules
    void OnServerLibraryLoaded();
    void OnClientLibraryLoaded();
    void OnFrameUpdate();
    void OnCreateMove(CUserCmd* cmd, bool& gameActive);
    void OnSqvmCreated(void* c_sqvm);
    void OnSqvmDestroying(void* c_sqvm);
    
    // Configuration management
    void OnConfigChanged();
    void ReloadAllConfigs();
    
    // Statistics and debugging
    size_t GetModuleCount() const { return modules_.size(); }
    std::vector<std::string> GetModuleNames() const;
    void PrintModuleStatus() const;
    
    // Enable/disable modules
    void EnableModule(const std::string& moduleName);
    void DisableModule(const std::string& moduleName);
    bool IsModuleEnabled(const std::string& moduleName) const;
    
private:
    PluginCallbacksManager() = default;
    ~PluginCallbacksManager() = default;
    PluginCallbacksManager(const PluginCallbacksManager&) = delete;
    PluginCallbacksManager& operator=(const PluginCallbacksManager&) = delete;
    
    void InitializeEventHandlers();
    
    std::vector<std::unique_ptr<IFeatureModule>> modules_;
    std::unordered_map<std::string, bool> module_enabled_state_;
    bool initialized_ = false;
};
