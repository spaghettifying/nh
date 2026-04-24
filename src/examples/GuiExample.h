#pragma once

/**
 * Simple example of how to use the external GUI system
 */
class GuiExample {
public:
    // Initialize and show the GUI
    static void ShowConfigWindow() {
        auto& gui = GuiManager::GetInstance();
        
        // Initialize if not already done
        if (!gui.IsInitialized()) {
            gui.Initialize();
        }
        
        // Show the window
        gui.ShowWindow();
    }
    
    // Hide the GUI window
    static void HideConfigWindow() {
        GuiManager::GetInstance().HideWindow();
    }
    
    // Toggle GUI visibility
    static void ToggleConfigWindow() {
        GuiManager::GetInstance().ToggleVisibility();
    }
    
    // Example: Open GUI from console command
    static void OnConsoleCommand() {
        ShowConfigWindow();
    }
};
