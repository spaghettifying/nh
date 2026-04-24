#pragma once

#include "../core/IFeatureModule.h"

/**
 * Debug Module
 * Handles debug output and logging
 */
class DebugModule : public IFeatureModule {
public:
    DebugModule() = default;
    virtual ~DebugModule() = default;
    
    // IFeatureModule interface
    void Initialize() override;
    void Shutdown() override;
    void OnServerLibraryLoaded() override;
    void OnClientLibraryLoaded() override;
    void OnSqvmCreated(void* c_sqvm) override;
    void OnSqvmDestroying(void* c_sqvm) override;
    void OnConfigChanged() override;
    
    const char* GetModuleName() const override { return "Debug"; }
    bool IsEnabled() const override { return enabled_; }
    void SetEnabled(bool enabled) override { enabled_ = enabled; }
    
private:
    void LogDebugInfo(const std::string& message, const std::string& context = "");
    
    bool enabled_ = false;
};
