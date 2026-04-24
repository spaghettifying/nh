#pragma once

#ifdef _WIN32
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <windows.h>
#include <thread>
#include <atomic>
#endif

#include <functional>
#include <memory>

class GuiManager {
public:
    static GuiManager& GetInstance();
    
    // Initialize the GUI system (creates external window)
    bool Initialize();
    
    // Cleanup GUI resources
    void Cleanup();
    
    // Show/Hide the external window
    void ShowWindow();
    void HideWindow();
    void ToggleVisibility();
    bool IsVisible() const { return visible_; }
    
private:
    GuiManager() = default;
    ~GuiManager() = default;
    
    // Singleton pattern
    GuiManager(const GuiManager&) = delete;
    GuiManager& operator=(const GuiManager&) = delete;
    
    // GUI thread function
    void GuiThreadFunction();
    
    // Render the main menu window
    void RenderMainWindow();
    
    // Render feature toggles
    void RenderFeatureToggles();
    
    // Render configuration options
    void RenderConfigOptions();
    
    // GUI state
    std::atomic<bool> visible_ = false;
    std::atomic<bool> should_close_ = false;
    bool initialized_ = false;
    
#ifdef _WIN32
    // GUI thread
    std::unique_ptr<std::thread> gui_thread_;
    
    // DirectX 11 resources for external window
    ID3D11Device* d3d_device_ = nullptr;
    ID3D11DeviceContext* d3d_device_context_ = nullptr;
    ID3D11RenderTargetView* main_render_target_view_ = nullptr;
    IDXGISwapChain* swap_chain_ = nullptr;
    
    // External window handle
    HWND external_window_ = nullptr;
    
    // Window class name
    static const char* WINDOW_CLASS_NAME;
    
    // Window procedure
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    // Create the external window
    bool CreateExternalWindow();
    
    // Initialize DirectX for the external window
    bool InitializeDirectX();
    
    // Cleanup DirectX resources
    void CleanupDirectX();
#endif
};
