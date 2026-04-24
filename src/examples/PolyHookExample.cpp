#include "PolyHookExample.h"
#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Virtuals/VFuncSwapHook.hpp>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

// Static member definitions
std::unique_ptr<PLH::x64Detour> PolyHookExample::detourHook = nullptr;
std::unique_ptr<PLH::VFuncSwapHook> PolyHookExample::vtableHook = nullptr;
uint64_t PolyHookExample::originalFunction = 0;

#ifdef _WIN32
// Example: Hook MessageBoxA from user32.dll
typedef int(WINAPI* MessageBoxA_t)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
MessageBoxA_t originalMessageBoxA = nullptr;

// Hooked MessageBox function
int WINAPI HookedMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    std::cout << "MessageBoxA hooked! Text: " << (lpText ? lpText : "NULL") << std::endl;
    
    // Modify the message or call original
    return originalMessageBoxA(hWnd, "Hooked by PolyHook!", lpCaption, uType);
}
#endif

bool PolyHookExample::HookDllFunction(const std::string& dllName, const std::string& functionName) {
#ifdef _WIN32
    // Get handle to the DLL
    HMODULE hModule = GetModuleHandleA(dllName.c_str());
    if (!hModule) {
        // Try to load the DLL if it's not already loaded
        hModule = LoadLibraryA(dllName.c_str());
        if (!hModule) {
            std::cerr << "Failed to load DLL: " << dllName << std::endl;
            return false;
        }
    }
    
    // Get the function address
    FARPROC functionAddress = GetProcAddress(hModule, functionName.c_str());
    if (!functionAddress) {
        std::cerr << "Failed to find function " << functionName << " in " << dllName << std::endl;
        return false;
    }
    
    std::cout << "Found " << functionName << " at address: 0x" << std::hex << (uint64_t)functionAddress << std::endl;
    
    // Example: Hook MessageBoxA specifically
    if (functionName == "MessageBoxA") {
        detourHook = std::make_unique<PLH::x64Detour>(
            (uint64_t)functionAddress,
            (uint64_t)&HookedMessageBoxA,
            (uint64_t*)&originalMessageBoxA
        );
        
        if (detourHook->hook()) {
            std::cout << "Successfully hooked " << functionName << " from " << dllName << std::endl;
            return true;
        } else {
            std::cerr << "Failed to hook " << functionName << " from " << dllName << std::endl;
            return false;
        }
    }
    
    // For other functions, you'd need to create appropriate hook functions
    // This is just a template showing how to get the address
    std::cout << "Function found but no hook implementation for: " << functionName << std::endl;
    return false;
#else
    std::cerr << "DLL hooking only supported on Windows" << std::endl;
    return false;
#endif
}

void PolyHookExample::InitializeHooks() {
    // Example 1: Hook MessageBoxA from user32.dll
    HookDllFunction("user32.dll", "MessageBoxA");
    
    // Example 2: Hook other DLL functions
    // HookDllFunction("kernel32.dll", "CreateFileA");
    // HookDllFunction("ntdll.dll", "NtCreateFile");
    
    // Example 3: Hook a function at a specific address (original method)
    // uint64_t targetAddress = 0x140001000;
    // detourHook = std::make_unique<PLH::x64Detour>(
    //     targetAddress,
    //     (uint64_t)&HookExampleFunction,
    //     &originalFunction
    // );
}

void PolyHookExample::CleanupHooks() {
    if (detourHook) {
        detourHook->unHook();
        detourHook.reset();
    }
    
    if (vtableHook) {
        vtableHook->unHook();
        vtableHook.reset();
    }
}

void PolyHookExample::HookExampleFunction() {
    // Your custom logic here
    std::cout << "Hooked function called!" << std::endl;
    
    // Call original function if needed
    if (originalFunction) {
        // Cast and call original function
        // Example: ((void(*)())originalFunction)();
    }
}

void PolyHookExample::CleanupHooks() {
    if (detourHook) {
        detourHook->unHook();
        detourHook.reset();
    }
    
    if (vtableHook) {
        vtableHook->unHook();
        vtableHook.reset();
    }
}

void PolyHookExample::HookExampleFunction() {
    // Your custom logic here
    std::cout << "Hooked function called!" << std::endl;
    
    // Call original function if needed
    if (originalFunction) {
        // Cast and call original function
        // Example: ((void(*)())originalFunction)();
    }
}
