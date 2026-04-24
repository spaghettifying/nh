#include "DllHooker.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>

// Example 1: Hook MessageBoxA
typedef int(WINAPI* MessageBoxA_t)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
MessageBoxA_t originalMessageBoxA = nullptr;

int WINAPI HookedMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    std::cout << "MessageBoxA intercepted! Original text: " << (lpText ? lpText : "NULL") << std::endl;
    
    // You can modify parameters or completely replace behavior
    std::string newText = "Hooked: " + std::string(lpText ? lpText : "NULL");
    return originalMessageBoxA(hWnd, newText.c_str(), lpCaption, uType);
}

// Example 2: Hook CreateFileA
typedef HANDLE(WINAPI* CreateFileA_t)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
                                     LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                     DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
CreateFileA_t originalCreateFileA = nullptr;

HANDLE WINAPI HookedCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                               LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                               DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    std::cout << "CreateFileA intercepted! File: " << (lpFileName ? lpFileName : "NULL") << std::endl;
    
    // Call original function
    return originalCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
                              dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

// Example 3: Hook any function in engine.dll (Titanfall 2 engine)
typedef void(*SomeEngineFunction_t)();
SomeEngineFunction_t originalEngineFunction = nullptr;

void HookedEngineFunction() {
    std::cout << "Engine function hooked!" << std::endl;
    
    // Your custom logic here
    
    // Call original if needed
    if (originalEngineFunction) {
        originalEngineFunction();
    }
}

#endif

class HookingExamples {
public:
    static void SetupHooks() {
#ifdef _WIN32
        // Hook Windows API functions
        bool success1 = DllHooker::HookFunction("user32.dll", "MessageBoxA", 
                                               HookedMessageBoxA, &originalMessageBoxA);
        
        bool success2 = DllHooker::HookFunction("kernel32.dll", "CreateFileA", 
                                               HookedCreateFileA, &originalCreateFileA);
        
        // Hook Titanfall 2 engine functions (replace "SomeFunctionName" with actual function)
        // bool success3 = DllHooker::HookFunction("engine.dll", "SomeFunctionName", 
        //                                        HookedEngineFunction, &originalEngineFunction);
        
        std::cout << "Hook setup results:" << std::endl;
        std::cout << "MessageBoxA: " << (success1 ? "SUCCESS" : "FAILED") << std::endl;
        std::cout << "CreateFileA: " << (success2 ? "SUCCESS" : "FAILED") << std::endl;
        
        // Test the hooks
        MessageBoxA(NULL, "Test message", "Test", MB_OK);
#endif
    }
    
    static void CleanupHooks() {
        DllHooker::CleanupAllHooks();
    }
};
