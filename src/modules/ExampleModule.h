#pragma once

#include "../core/IFeatureModule.h"
#include <chrono>

/**
 * Example module showing how to create new features
 * This module demonstrates:
 * - Configuration integration
 * - Event system usage
 * - Frame-based processing
 * - Enable/disable functionality
 */
class ExampleModule : public IFeatureModule {
public:
    ExampleModule() = default;
    virtual ~ExampleModule() = default;
    
    // IFeatureModule interface
    void Initialize() override;
    void Shutdown() override;
    void OnFrameUpdate() override;
    void OnClientLibraryLoaded() override;
    void OnConfigChanged() override;
    
    const char* GetModuleName() const override { return "Example"; }
    bool IsEnabled() const override { return enabled_; }
    void SetEnabled(bool enabled) override { enabled_ = enabled; }
    
    // Custom functionality
    void DoSomething();
    void PublishExampleEvent();

private:
    bool enabled_ = true;
    std::chrono::steady_clock::time_point last_action_time_;
    int action_count_ = 0;
};
