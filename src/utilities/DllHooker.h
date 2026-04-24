#pragma once

#include <polyhook2/Detour/x64Detour.hpp>
#include <string>
#include <memory>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#endif

/**
 * Generic DLL Function Hooker
 * Allows hooking functions by DLL name and function name
 */
class DllHooker {
public:
    // Hook a function and provide a callback
    template<typename FuncType>
    static bool HookFunction(const std::string& dllName, 
                           const std::string& functionName,
                           FuncType hookFunction,
                           FuncType* originalFunction);
    
    // Unhook a specific function
    static bool UnhookFunction(const std::string& dllName, const std::string& functionName);
    
    // Cleanup all hooks
    static void CleanupAllHooks();
    
    // Get function address from DLL
    static void* GetFunctionAddress(const std::string& dllName, const std::string& functionName);

private:
    // Store hooks by DLL::Function name
    static std::unordered_map<std::string, std::unique_ptr<PLH::x64Detour>> hooks_;
    
    // Helper to create hook key
    static std::string CreateHookKey(const std::string& dllName, const std::string& functionName);
};

// Template implementation
template<typename FuncType>
bool DllHooker::HookFunction(const std::string& dllName, 
                           const std::string& functionName,
                           FuncType hookFunction,
                           FuncType* originalFunction) {
#ifdef _WIN32
    void* targetAddress = GetFunctionAddress(dllName, functionName);
    if (!targetAddress) {
        return false;
    }
    
    std::string hookKey = CreateHookKey(dllName, functionName);
    
    // Create the detour hook
    auto detour = std::make_unique<PLH::x64Detour>(
        (uint64_t)targetAddress,
        (uint64_t)hookFunction,
        (uint64_t*)originalFunction
    );
    
    if (detour->hook()) {
        hooks_[hookKey] = std::move(detour);
        return true;
    }
    
    return false;
#else
    return false; // Not supported on non-Windows platforms
#endif
}
