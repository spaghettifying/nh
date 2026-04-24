#include "DllHooker.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

// Static member definition
std::unordered_map<std::string, std::unique_ptr<PLH::x64Detour>> DllHooker::hooks_;

void* DllHooker::GetFunctionAddress(const std::string& dllName, const std::string& functionName) {
#ifdef _WIN32
    // Get handle to the DLL
    HMODULE hModule = GetModuleHandleA(dllName.c_str());
    if (!hModule) {
        // Try to load the DLL if it's not already loaded
        hModule = LoadLibraryA(dllName.c_str());
        if (!hModule) {
            std::cerr << "Failed to load DLL: " << dllName << std::endl;
            return nullptr;
        }
    }
    
    // Get the function address
    FARPROC functionAddress = GetProcAddress(hModule, functionName.c_str());
    if (!functionAddress) {
        std::cerr << "Failed to find function " << functionName << " in " << dllName << std::endl;
        return nullptr;
    }
    
    std::cout << "Found " << functionName << " at address: 0x" << std::hex << (uint64_t)functionAddress << std::endl;
    return (void*)functionAddress;
#else
    std::cerr << "DLL function resolution only supported on Windows" << std::endl;
    return nullptr;
#endif
}

bool DllHooker::UnhookFunction(const std::string& dllName, const std::string& functionName) {
    std::string hookKey = CreateHookKey(dllName, functionName);
    
    auto it = hooks_.find(hookKey);
    if (it != hooks_.end()) {
        it->second->unHook();
        hooks_.erase(it);
        return true;
    }
    
    return false;
}

void DllHooker::CleanupAllHooks() {
    for (auto& [key, hook] : hooks_) {
        hook->unHook();
    }
    hooks_.clear();
}

std::string DllHooker::CreateHookKey(const std::string& dllName, const std::string& functionName) {
    return dllName + "::" + functionName;
}
