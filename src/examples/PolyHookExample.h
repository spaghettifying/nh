#pragma once

#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Virtuals/VFuncSwapHook.hpp>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

class PolyHookExample {
public:
    static void InitializeHooks();
    static void CleanupHooks();
    
    // Hook a function by DLL name and function name
    static bool HookDllFunction(const std::string& dllName, const std::string& functionName);

private:
    // Example function hook
    static void HookExampleFunction();
    
    // Detour hook example
    static std::unique_ptr<PLH::x64Detour> detourHook;
    
    // VTable hook example  
    static std::unique_ptr<PLH::VFuncSwapHook> vtableHook;
    
    // Original function pointers
    static uint64_t originalFunction;
};
