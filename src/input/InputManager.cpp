#include "InputManager.h"
#include "../gui/GuiManager.h"
#include "../pch.h"
#include "../r2sdk/classes/CInputSystem.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {
void LogInputMessage(const std::string& msg) {
    if (g_nssys && g_handle) {
        g_nssys->Log(g_handle, LogLevel::INFO, msg.c_str());
    }
    std::cout << msg << std::endl;
}
}

InputManager& InputManager::GetInstance() {
    static InputManager instance;
    return instance;
}

bool InputManager::Initialize() {
    if (initialized_) {
        return true;
    }
    
    // Register default key bindings
    RegisterKeyCallback(KEY_INSERT, []() {
        GuiManager::GetInstance().ToggleVisibility();
        LogInputMessage("GUI toggled via INSERT");
    });

    initialized_ = true;
    LogInputMessage("InputManager initialized (bindings: INSERT, A, F10)");
    return true;
}

void InputManager::Cleanup() {
    keyCallbacks_.clear();
    previousKeyStates_.clear();
    initialized_ = false;
    LogInputMessage("InputManager cleaned up");
}

void InputManager::RegisterKeyCallback(int virtualKey, std::function<void()> callback) {
    keyCallbacks_[virtualKey] = callback;
    previousKeyStates_[virtualKey] = false;
}

void InputManager::ProcessInput() {
#ifdef _WIN32
    static bool loggedNotInitialized = false;
    static bool loggedNoInputSystem = false;

    if (!initialized_) {
        if (!loggedNotInitialized) {
            LogInputMessage("ProcessInput skipped: InputManager not initialized");
            loggedNotInitialized = true;
        }
        return;
    }

    if (!InputSystem) {
        if (!loggedNoInputSystem) {
            LogInputMessage("ProcessInput skipped: InputSystem is null");
            loggedNoInputSystem = true;
        }
        return;
    }
    
    for (auto& [key, callback] : keyCallbacks_) {
      bool currentState = IsKeyPressed(key);
      bool previousState = previousKeyStates_[key];

      // Trigger on key press (edge detection)
      if (currentState && !previousState) {
        callback();
      }
        
        previousKeyStates_[key] = currentState;
    }
#endif
}

bool InputManager::IsKeyPressed(int virtualKey) {
#ifdef _WIN32
    if (!InputSystem) {
        return false;
    }

    return InputSystem->IsButtonDown(static_cast<ButtonCode_t>(virtualKey));
#else
    return false;
#endif
}
