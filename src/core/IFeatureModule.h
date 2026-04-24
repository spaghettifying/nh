#pragma once

#include <memory>
#include <vector>
#include <functional>

// Forward declarations
class IFeatureModule;
class EventSystem;
class CUserCmd;

/**
 * Base class for plugin feature modules
 * Each feature (like UID logging, NetVar dumping, etc.) should inherit from this
 */
class IFeatureModule {
public:
    virtual ~IFeatureModule() = default;
    
    // Lifecycle methods
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
    
    // Frame processing
    virtual void OnFrameUpdate() {}
    virtual void OnCreateMove(CUserCmd* /*cmd*/, bool& /*gameActive*/) {}
    
    // Library loading events
    virtual void OnServerLibraryLoaded() {}
    virtual void OnClientLibraryLoaded() {}
    
    // Squirrel VM events
    virtual void OnSqvmCreated(void* /*c_sqvm*/) {}
    virtual void OnSqvmDestroying(void* /*c_sqvm*/) {}
    
    // Configuration
    virtual void OnConfigChanged() {}
    
    // Module info
    virtual const char* GetModuleName() const = 0;
    virtual bool IsEnabled() const { return true; }
    virtual void SetEnabled(bool /*enabled*/) {}
};
