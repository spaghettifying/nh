#include "IPluginCallbacks.h"
#include "../../../CppPluginExample.h"
#include "../../../pch.h"
#include "../../../Config.h"
#include "../../../gui/GuiManager.h"
#include "../../../input/InputManager.h"
#include "../../../core/PluginCallbacksManager.h"
#include "../../../core/EventSystem.h"
#include "../../../renderer/renderer.h"
#include "../../../hooks/PaintTraverseHook.h"
#include "../../../hooks/CreateMoveHook.h"

// Include feature modules
#include "../../../modules/LobbyLoggingModule.h"
#include "../../../modules/DebugModule.h"
#include "../../../modules/NetVarModule.h"
#include "../../../modules/ESP.h"
#include "../../../modules/AirstuckModule.h"
#include "../../../modules/Speed.h"
#include "../../../modules/Aimbot.h"

#include <fstream>
#include <set>
#include <memory>
#include <iostream>

class CPluginCallbacks : public IPluginCallbacks {
private:
  int frameCount;
  PluginCallbacksManager* manager_;

public:
  CPluginCallbacks() : frameCount(0), manager_(&PluginCallbacksManager::GetInstance()) {}

  void Init(HMODULE northstarModule, const PluginNorthstarData *initData,
            bool /*reloaded*/) {
    InitCppExamplePlugin(initData->pluginHandle, northstarModule);
    frameCount = 0;
    
    std::cout << "=== Enhanced Plugin Callbacks Initializing ===" << std::endl;
    
    // Fallback initialization in case client.dll was loaded before this plugin
    // and OnLibraryLoaded("client.dll") is not fired for us.
    Initialize::InitPointers();
    if (g_nssys && g_handle) {
      g_nssys->Log(g_handle, LogLevel::INFO,
                   "InitPointers() called from Init fallback");
    }

    // Initialize systems
    InitializeSystems();

    if (PaintTraverseHook::Initialize()) {
      if (g_nssys && g_handle) {
        g_nssys->Log(g_handle, LogLevel::INFO,
                     "PaintTraverse hook installed");
      }
    } else {
      if (g_nssys && g_handle) {
        g_nssys->Log(g_handle, LogLevel::WARN,
                     "PaintTraverse hook not installed yet");
      }
    }

    if (CreateMoveHook::Initialize()) {
      if (g_nssys && g_handle) {
        g_nssys->Log(g_handle, LogLevel::INFO, "CreateMove hook installed");
      }
    } else {
      if (g_nssys && g_handle) {
        g_nssys->Log(g_handle, LogLevel::WARN,
                     "CreateMove hook not installed yet");
      }
    }
    
    // Register feature modules
    RegisterFeatureModules();
    
    // Initialize the plugin manager
    manager_->Initialize();
    
    // Log configuration on startup
    g_config.logCurrentSettings();
    
    std::cout << "=== Enhanced Plugin Callbacks Initialized Successfully ===" << std::endl;
  }

  void OnLibraryLoaded(HMODULE /*module*/, const char *name) {
    std::cout << "Library loaded: " << name << std::endl;

    // Retry pointer/interface initialization whenever a relevant client-side
    // module appears, since load order varies under Northstar/Wine.
    if (strcmp(name, "client.dll") == 0 || strcmp(name, "engine.dll") == 0 ||
        strcmp(name, "inputsystem.dll") == 0 ||
        strcmp(name, "vguimatsurface.dll") == 0 ||
        strcmp(name, "vgui2.dll") == 0) {
      Initialize::InitPointers();
    }
    
    if (strcmp(name, "server.dll") == 0) {
      std::cout << "Server library detected, forwarding to modules..." << std::endl;
      manager_->OnServerLibraryLoaded();
    }

    if (strcmp(name, "client.dll") == 0) {
      std::cout << "Client library detected, initializing client-side systems..." << std::endl;
      
      // Initialize core systems first
      Initialize::InitPointers();
      
      // Then forward to modules
      manager_->OnClientLibraryLoaded();
    }
  }

  void Finalize() {
    // Finalize is called after plugins are loaded; do not teardown here.
    std::cout << "=== Plugin Finalized (post-load) ===" << std::endl;
  }

  void Unload() {
    std::cout << "=== Plugin Unloading ===" << std::endl;

    // Cleanup systems in reverse order
    CreateMoveHook::Shutdown();
    PaintTraverseHook::Shutdown();

    if (manager_) {
      manager_->Shutdown();
    }

    // Cleanup GUI and input systems
    GuiManager::GetInstance().Cleanup();
    InputManager::GetInstance().Cleanup();
    
    std::cout << "=== Plugin Unloaded ===" << std::endl;
  }

  void OnSqvmCreated(void *c_sqvm) {
    std::cout << "Squirrel VM created, forwarding to modules..." << std::endl;
    manager_->OnSqvmCreated(c_sqvm);
  }

  void OnSqvmDestroying(void *c_sqvm) {
    std::cout << "Squirrel VM destroying, forwarding to modules..." << std::endl;
    manager_->OnSqvmDestroying(c_sqvm);
  }

  void RunFrame() {
    // Process input first so GUI hotkeys still work in states where game interfaces are not ready.
    InputManager::GetInstance().ProcessInput();

    // Keep retrying until core client interfaces are resolved.
    if (!g_Interfaces.Client || !g_Interfaces.ClientEntityList ||
        !g_Interfaces.EngineClient || !MatSystemSurface || !g_Interfaces.Panel) {
      static int retryCounter = 0;
      ++retryCounter;
      if (retryCounter % 120 == 0) {
        Initialize::InitPointers();
        if (!PaintTraverseHook::IsInstalled()) {
          PaintTraverseHook::Initialize();
        }
        if (!CreateMoveHook::IsInstalled()) {
          CreateMoveHook::Initialize();
        }
      }
    }

    if (!PaintTraverseHook::IsInstalled() && g_Interfaces.Panel) {
      PaintTraverseHook::Initialize();
    }

    if (!CreateMoveHook::IsInstalled() && g_Interfaces.Client) {
      CreateMoveHook::Initialize();
    }

    if (g_config.isRendererSelfTestEnabled()) {
      Renderer::DrawSelfTestOverlay();
    }

    // Check if ClientEntityList is available
    if (!g_Interfaces.ClientEntityList)
      return;
      
    frameCount++;
    
    // Forward frame update to all modules
    manager_->OnFrameUpdate();
    
    // Periodic status updates (every 10000 frames ≈ every 3 minutes at 60fps)
    if (frameCount % 10000 == 0) {
      std::cout << "Frame: " << frameCount << " - Plugin running normally" << std::endl;
      manager_->PrintModuleStatus();
    }
  }

private:
  void InitializeSystems() {
    std::cout << "Initializing core systems..." << std::endl;
    
    // Initialize GUI system
    GuiManager::GetInstance().Initialize();
    
    // Initialize input system
    InputManager::GetInstance().Initialize();
    
    // Initialize event system (it's a singleton, so this just ensures it's ready)
    EventSystem::GetInstance();
    
    std::cout << "Core systems initialized successfully" << std::endl;
  }
  
  void RegisterFeatureModules() {
    std::cout << "Registering feature modules..." << std::endl;
    
    // Register lobby logging module
    manager_->RegisterModule(std::make_unique<LobbyLoggingModule>());
    
    // Register debug module
    manager_->RegisterModule(std::make_unique<DebugModule>());
    
    // Register NetVar module
    manager_->RegisterModule(std::make_unique<NetVarModule>());

    // Register ESP module
    manager_->RegisterModule(std::make_unique<ESP>());

    // Register airstuck module
    manager_->RegisterModule(std::make_unique<Airstuck>());

    // Register speed module
    manager_->RegisterModule(std::make_unique<Speed>());

    // Register aimbot module
    manager_->RegisterModule(std::make_unique<Aimbot>());
    
    std::cout << "Feature modules registered: " << manager_->GetModuleCount() << " total" << std::endl;
    
    // Print module names
    auto moduleNames = manager_->GetModuleNames();
    for (const auto& name : moduleNames) {
      std::cout << "  - " << name << std::endl;
    }
  }
};

EXPOSE_SINGLE_INTERFACE(CPluginCallbacks, IPluginCallbacks,
                        PLUGIN_CALLBACKS_VERSION)
