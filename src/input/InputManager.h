#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <functional>
#include <unordered_map>

class InputManager {
public:
    static InputManager& GetInstance();
    
    // Initialize input handling
    bool Initialize();
    
    // Cleanup
    void Cleanup();
    
    // Register a key callback
    void RegisterKeyCallback(int virtualKey, std::function<void()> callback);
    
    // Process input (call this regularly)
    void ProcessInput();
    
    // Check if a key is currently pressed
    bool IsKeyPressed(int virtualKey);
    
private:
    InputManager() = default;
    ~InputManager() = default;
    
    // Singleton pattern
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    
    // Key callbacks
    std::unordered_map<int, std::function<void()>> keyCallbacks_;
    
    // Key states for edge detection
    std::unordered_map<int, bool> previousKeyStates_;
    
    bool initialized_ = false;
};
