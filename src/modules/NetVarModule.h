#pragma once

#include "../core/IFeatureModule.h"

/**
 * NetVar Module
 * Handles NetVar dumping and management
 */
class NetVarModule : public IFeatureModule {
public:
    NetVarModule() = default;
    virtual ~NetVarModule() = default;
    
    // IFeatureModule interface
    void Initialize() override;
    void Shutdown() override;
    void OnClientLibraryLoaded() override;
    void OnConfigChanged() override;
    
    const char* GetModuleName() const override { return "NetVar"; }
    bool IsEnabled() const override { return enabled_; }
    void SetEnabled(bool enabled) override { enabled_ = enabled; }
    
    // NetVar specific functions
    void DumpNetVars();
    void RefreshNetVars();
    
private:
    bool enabled_ = false;
    bool dump_on_client_load_ = true;
};
