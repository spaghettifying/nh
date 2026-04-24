#include "GuiManager.h"
#include "../Config.h"
#include "../core/EventSystem.h"
#include <iostream>

#ifdef _WIN32
#include <d3d11.h>
#include <dxgi.h>

// Static member definitions
const char* GuiManager::WINDOW_CLASS_NAME = "PluginConfigWindow";

// External ImGui WndProc handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

GuiManager& GuiManager::GetInstance() {
    static GuiManager instance;
    return instance;
}

bool GuiManager::Initialize() {
#ifdef _WIN32
    if (initialized_) {
        return true;
    }
    
    // Create the external window
    if (!CreateExternalWindow()) {
        std::cerr << "Failed to create external window" << std::endl;
        return false;
    }
    
    // Initialize DirectX for the window
    if (!InitializeDirectX()) {
        std::cerr << "Failed to initialize DirectX" << std::endl;
        return false;
    }
    
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Note: ImGuiConfigFlags_DockingEnable not available in this ImGui version
    
    // Setup ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(external_window_);
    ImGui_ImplDX11_Init(d3d_device_, d3d_device_context_);
    
    // Start the GUI thread
    should_close_ = false;
    gui_thread_ = std::make_unique<std::thread>(&GuiManager::GuiThreadFunction, this);
    
    initialized_ = true;
    std::cout << "External GUI window initialized successfully" << std::endl;
    return true;
#else
    std::cerr << "GUI only supported on Windows" << std::endl;
    return false;
#endif
}

void GuiManager::Cleanup() {
#ifdef _WIN32
    if (!initialized_) return;
    
    // Signal thread to close
    should_close_ = true;
    
    // Wait for GUI thread to finish
    if (gui_thread_ && gui_thread_->joinable()) {
        gui_thread_->join();
        gui_thread_.reset();
    }
    
    // Cleanup ImGui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    // Cleanup DirectX
    CleanupDirectX();
    
    // Destroy window
    if (external_window_) {
        DestroyWindow(external_window_);
        external_window_ = nullptr;
    }
    
    // Unregister window class
    UnregisterClassA(WINDOW_CLASS_NAME, GetModuleHandle(nullptr));
    
    initialized_ = false;
    std::cout << "GUI cleaned up" << std::endl;
#endif
}

void GuiManager::ShowWindow() {
    visible_ = true;
#ifdef _WIN32
    if (external_window_) {
        ::ShowWindow(external_window_, SW_SHOW);
        SetForegroundWindow(external_window_);
    }
#endif
}

void GuiManager::HideWindow() {
    visible_ = false;
#ifdef _WIN32
    if (external_window_) {
        ::ShowWindow(external_window_, SW_HIDE);
    }
#endif
}

void GuiManager::ToggleVisibility() {
    if (visible_) {
        HideWindow();
    } else {
        ShowWindow();
    }
}

#ifdef _WIN32
bool GuiManager::CreateExternalWindow() {
    // Register window class
    WNDCLASSEXW wc;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0L;
    wc.cbWndExtra = 0L;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"PluginConfigWindow";
    wc.hIconSm = nullptr;
    
    if (!RegisterClassExW(&wc)) {
        std::cerr << "Failed to register window class" << std::endl;
        return false;
    }
    
    // Create window
    external_window_ = CreateWindowW(
        L"PluginConfigWindow",
        L"Plugin Configuration",
        WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600,
        nullptr, nullptr, wc.hInstance, nullptr
    );
    
    if (!external_window_) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }
    
    // Store this pointer in window user data for WndProc access
    SetWindowLongPtr(external_window_, GWLP_USERDATA, (LONG_PTR)this);
    
    return true;
}

bool GuiManager::InitializeDirectX() {
    // Create device and swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = external_window_;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    
    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd,
        &swap_chain_, &d3d_device_, &featureLevel, &d3d_device_context_
    );
    
    if (res != S_OK) {
        std::cerr << "Failed to create DirectX device and swap chain" << std::endl;
        return false;
    }
    
    // Create render target view
    ID3D11Texture2D* pBackBuffer;
    swap_chain_->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer) {
        d3d_device_->CreateRenderTargetView(pBackBuffer, nullptr, &main_render_target_view_);
        pBackBuffer->Release();
    }
    
    return true;
}

void GuiManager::CleanupDirectX() {
    if (main_render_target_view_) {
        main_render_target_view_->Release();
        main_render_target_view_ = nullptr;
    }
    if (swap_chain_) {
        swap_chain_->Release();
        swap_chain_ = nullptr;
    }
    if (d3d_device_context_) {
        d3d_device_context_->Release();
        d3d_device_context_ = nullptr;
    }
    if (d3d_device_) {
        d3d_device_->Release();
        d3d_device_ = nullptr;
    }
}

void GuiManager::GuiThreadFunction() {
    // Show window initially hidden
    ::ShowWindow(external_window_, SW_HIDE);
    UpdateWindow(external_window_);
    
    // Main message loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    
    while (!should_close_ && msg.message != WM_QUIT) {
        // Poll and handle messages
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        
        // Only render if window is visible
        if (!visible_) {
            Sleep(16); // ~60 FPS when hidden
            continue;
        }
        
        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        // Render our GUI
        RenderMainWindow();
        
        // Rendering
        ImGui::Render();
        
        // Clear and render
        const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        d3d_device_context_->OMSetRenderTargets(1, &main_render_target_view_, nullptr);
        d3d_device_context_->ClearRenderTargetView(main_render_target_view_, clear_color);
        
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        
        // Present
        swap_chain_->Present(1, 0); // Present with vsync
    }
}

LRESULT WINAPI GuiManager::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Get GuiManager instance from window user data
    GuiManager* gui_manager = (GuiManager*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    
    switch (msg) {
    case WM_SIZE:
        if (gui_manager && gui_manager->d3d_device_ && gui_manager->swap_chain_ && wParam != SIZE_MINIMIZED) {
            // Release only render target view, keep device/swapchain alive.
            if (gui_manager->main_render_target_view_) {
                gui_manager->main_render_target_view_->Release();
                gui_manager->main_render_target_view_ = nullptr;
            }

            gui_manager->swap_chain_->ResizeBuffers(
                0,
                (UINT)LOWORD(lParam),
                (UINT)HIWORD(lParam),
                DXGI_FORMAT_UNKNOWN,
                0);

            // Recreate render target view
            ID3D11Texture2D* pBackBuffer = nullptr;
            if (SUCCEEDED(gui_manager->swap_chain_->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))) && pBackBuffer) {
                gui_manager->d3d_device_->CreateRenderTargetView(
                    pBackBuffer,
                    nullptr,
                    &gui_manager->main_render_target_view_);
                pBackBuffer->Release();
            }
        }
        return 0;
        
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;

    case WM_KEYDOWN:
        if (wParam == VK_INSERT && gui_manager) {
            gui_manager->ToggleVisibility();
            return 0;
        }
        break;
        
    case WM_CLOSE:
        if (gui_manager) {
            gui_manager->HideWindow();
        }
        return 0;
        
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
#endif

void GuiManager::RenderMainWindow() {
    // Set window properties
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
    
    // Use a local bool for ImGui visibility control
    bool show_window = visible_.load();
    if (ImGui::Begin("Plugin Configuration", &show_window)) {
        // Update visibility state if user closed window
        if (!show_window) {
            visible_ = false;
        }
        
        // Plugin info
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Northstar C++ Plugin");
        ImGui::Separator();
        
        // Tabs for different sections
        if (ImGui::BeginTabBar("ConfigTabs")) {
            if (ImGui::BeginTabItem("Features")) {
                RenderFeatureToggles();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Configuration")) {
                RenderConfigOptions();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("About")) {
                ImGui::Text("C++ Plugin Template");
                ImGui::Text("Version: 1.0.0");
                ImGui::Text("For Titanfall 2 / Northstar");
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

void GuiManager::RenderFeatureToggles() {
    // Use the global config instance
    extern Config g_config;
    
    ImGui::Text("Feature Toggles:");
    ImGui::Separator();
    
    // Lobby Logging toggle
    bool lobbyLogging = g_config.isLobbyLoggingEnabled();
    if (ImGui::Checkbox("Lobby Logging", &lobbyLogging)) {
        g_config.setBool("lobby_logging_enabled", lobbyLogging);
        g_config.save();
        EventSystem::GetInstance().Publish(EventSystem::Events::CONFIG_CHANGED);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Log lobby player information");
    }
    
    // Debug output toggle
    bool debugOutput = g_config.isDebugLoggingEnabled();
    if (ImGui::Checkbox("Debug Output", &debugOutput)) {
        g_config.setBool("debug_logging_enabled", debugOutput);
        g_config.save();
        EventSystem::GetInstance().Publish(EventSystem::Events::CONFIG_CHANGED);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable debug console output");
    }
    
    // NetVar dump toggle
    bool netvarDump = g_config.isNetVarDumpEnabled();
    if (ImGui::Checkbox("NetVar Dump", &netvarDump)) {
        g_config.setBool("netvar_dump_enabled", netvarDump);
        g_config.save();
        EventSystem::GetInstance().Publish(EventSystem::Events::CONFIG_CHANGED);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable NetVar dumping on startup");
    }

    // ESP toggle
    bool espEnabled = g_config.isESPEnabled();
    if (ImGui::Checkbox("ESP", &espEnabled)) {
        g_config.setBool("esp_enabled", espEnabled);
        g_config.save();
        EventSystem::GetInstance().Publish(EventSystem::Events::CONFIG_CHANGED);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable ESP rendering");
    }

    // Aimbot toggle
    bool aimbotEnabled = g_config.getBool("aimbot_enabled");
    if (ImGui::Checkbox("Aimbot", &aimbotEnabled)) {
        g_config.setBool("aimbot_enabled", aimbotEnabled);
        g_config.save();
        EventSystem::GetInstance().Publish(EventSystem::Events::CONFIG_CHANGED);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable aimbot module (hold RMB)");
    }

    // Airstuck toggle
    bool airstuckEnabled = g_config.getBool("airstuck_enabled");
    if (ImGui::Checkbox("Airstuck", &airstuckEnabled)) {
        g_config.setBool("airstuck_enabled", airstuckEnabled);
        g_config.save();
        EventSystem::GetInstance().Publish(EventSystem::Events::CONFIG_CHANGED);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable airstuck module (hold ALT)");
    }

    // Speed module toggle
    bool speedEnabled = g_config.getBool("speed_enabled");
    if (ImGui::Checkbox("Speed Module", &speedEnabled)) {
        g_config.setBool("speed_enabled", speedEnabled);
        g_config.save();
        EventSystem::GetInstance().Publish(EventSystem::Events::CONFIG_CHANGED);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable speed module (uses Speed value)");
    }

    // Renderer self-test toggle
    bool rendererSelfTest = g_config.isRendererSelfTestEnabled();
    if (ImGui::Checkbox("Renderer Self Test", &rendererSelfTest)) {
        g_config.setBool("renderer_self_test_enabled", rendererSelfTest);
        g_config.save();
        EventSystem::GetInstance().Publish(EventSystem::Events::CONFIG_CHANGED);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Draws a test box/cross/text to validate CMatSystemSurface rendering");
    }
    
    // Module list toggle
    bool moduleList = g_config.isDisplayModuleListEnabled();
    if (ImGui::Checkbox("Display Module List", &moduleList)) {
        g_config.setBool("display_module_list", moduleList);
        g_config.save();
        EventSystem::GetInstance().Publish(EventSystem::Events::CONFIG_CHANGED);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Display the module list in top left");
    }

    ImGui::Spacing();
    
    // Action buttons
    if (ImGui::Button("Save Config")) {
        g_config.save();
        std::cout << "Configuration saved!" << std::endl;
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Reload Config")) {
        g_config.reload();
        std::cout << "Configuration reloaded!" << std::endl;
    }
}

void GuiManager::RenderConfigOptions() {
    // Use the global config instance
    extern Config g_config;
    
    ImGui::Text("Configuration Options:");
    ImGui::Separator();
    
    // Lobby log interval
    int logInterval = g_config.getLobbyLogInterval();
    if (ImGui::SliderInt("Lobby Log Interval (seconds)", &logInterval, 1, 60)) {
        g_config.setInt("lobby_log_interval", logInterval);
        g_config.save();
    }
    
    // Log file path
    std::string logPath = g_config.getLobbyLogFile();
    char logPathBuffer[256];
    strncpy(logPathBuffer, logPath.c_str(), sizeof(logPathBuffer) - 1);
    logPathBuffer[sizeof(logPathBuffer) - 1] = '\0';
    
    if (ImGui::InputText("Log File Path", logPathBuffer, sizeof(logPathBuffer))) {
        g_config.setString("lobby_log_file", std::string(logPathBuffer));
        g_config.save();
    }

    std::string title = g_config.getString("title");
    char titleBuffer[256];
    strncpy(titleBuffer, title.c_str(), sizeof(titleBuffer) - 1);
    titleBuffer[sizeof(titleBuffer) - 1] = '\0';

    if (ImGui::InputText("Title", titleBuffer, sizeof(titleBuffer))) {
      g_config.setString("title", std::string(titleBuffer));
      g_config.save();
    }

    float speed = g_config.getFloat("speed");
    if (ImGui::InputFloat("Speed", &speed, 0.01f, 0.10f, "%.3f")) {
        if (speed < 0.0f) {
            speed = 0.0f;
        }
        g_config.setFloat("speed", speed);
        g_config.save();
    }

    float aimbotFov = g_config.getFloat("aimbot_fov");
    if (ImGui::InputFloat("Aimbot FOV", &aimbotFov, 0.5f, 2.0f, "%.2f")) {
        if (aimbotFov < 0.1f) {
            aimbotFov = 0.1f;
        }
        g_config.setFloat("aimbot_fov", aimbotFov);
        g_config.save();
    }

    float aimbotSmooth = g_config.getFloat("aimbot_smooth");
    if (ImGui::InputFloat("Aimbot Smooth", &aimbotSmooth, 0.5f, 2.0f, "%.2f")) {
        if (aimbotSmooth < 1.0f) {
            aimbotSmooth = 1.0f;
        }
        g_config.setFloat("aimbot_smooth", aimbotSmooth);
        g_config.save();
    }


    ImGui::Spacing();
    
    // GUI settings
    ImGui::Text("GUI Settings:");
    ImGui::Separator();
    
    // GUI opacity
    static float guiOpacity = 1.0f;
    if (ImGui::SliderFloat("GUI Opacity", &guiOpacity, 0.1f, 1.0f)) {
        ImGui::GetStyle().Alpha = guiOpacity;
    }
    
    // GUI theme
    if (ImGui::Button("Dark Theme")) {
        ImGui::StyleColorsDark();
    }
    ImGui::SameLine();
    if (ImGui::Button("Light Theme")) {
        ImGui::StyleColorsLight();
    }
    ImGui::SameLine();
    if (ImGui::Button("Classic Theme")) {
        ImGui::StyleColorsClassic();
    }
    
    ImGui::Spacing();
    
    // Window controls
    if (ImGui::Button("Hide Window")) {
        HideWindow();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Position")) {
        ImGui::SetWindowPos("Plugin Configuration", ImVec2(100, 100));
    }
}
