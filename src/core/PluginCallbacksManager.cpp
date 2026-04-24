#include "PluginCallbacksManager.h"
#include "../Config.h"
#include <iostream>
#include <algorithm>

PluginCallbacksManager& PluginCallbacksManager::GetInstance() {
    static PluginCallbacksManager instance;
    return instance;
}

void PluginCallbacksManager::RegisterModule(std::unique_ptr<IFeatureModule> module) {
    if (!module) {
        std::cerr << "Attempted to register null module" << std::endl;
        return;
    }
    
    const std::string moduleName = module->GetModuleName();
    
    // Check if module already exists
    auto it = std::find_if(modules_.begin(), modules_.end(),
        [&moduleName](const std::unique_ptr<IFeatureModule>& mod) {
            return std::string(mod->GetModuleName()) == moduleName;
        });
    
    if (it != modules_.end()) {
        std::cerr << "Module " << moduleName << " is already registered" << std::endl;
        return;
    }
    
    std::cout << "Registering module: " << moduleName << std::endl;
    
    // Initialize if manager is already initialized
    if (initialized_) {
        module->Initialize();
    }
    
    modules_.push_back(std::move(module));
    module_enabled_state_[moduleName] = true; // Default to enabled
}

void PluginCallbacksManager::UnregisterModule(const std::string& moduleName) {
    auto it = std::find_if(modules_.begin(), modules_.end(),
        [&moduleName](const std::unique_ptr<IFeatureModule>& mod) {
            return std::string(mod->GetModuleName()) == moduleName;
        });
    
    if (it != modules_.end()) {
        std::cout << "Unregistering module: " << moduleName << std::endl;
        (*it)->Shutdown();
        modules_.erase(it);
        module_enabled_state_.erase(moduleName);
    }
}

IFeatureModule* PluginCallbacksManager::GetModule(const std::string& moduleName) {
    auto it = std::find_if(modules_.begin(), modules_.end(),
        [&moduleName](const std::unique_ptr<IFeatureModule>& mod) {
            return std::string(mod->GetModuleName()) == moduleName;
        });
    
    return (it != modules_.end()) ? it->get() : nullptr;
}

void PluginCallbacksManager::Initialize() {
    if (initialized_) {
        return;
    }
    
    std::cout << "Initializing PluginCallbacksManager with " << modules_.size() << " modules" << std::endl;
    
    // Initialize event handlers
    InitializeEventHandlers();
    
    // Initialize all registered modules
    for (auto& module : modules_) {
        try {
            std::cout << "Initializing module: " << module->GetModuleName() << std::endl;
            module->Initialize();
        } catch (const std::exception& e) {
            std::cerr << "Error initializing module " << module->GetModuleName() << ": " << e.what() << std::endl;
        }
    }
    
    initialized_ = true;
}

void PluginCallbacksManager::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "Shutting down PluginCallbacksManager" << std::endl;
    
    // Shutdown all modules in reverse order
    for (auto it = modules_.rbegin(); it != modules_.rend(); ++it) {
        try {
            std::cout << "Shutting down module: " << (*it)->GetModuleName() << std::endl;
            (*it)->Shutdown();
        } catch (const std::exception& e) {
            std::cerr << "Error shutting down module " << (*it)->GetModuleName() << ": " << e.what() << std::endl;
        }
    }
    
    initialized_ = false;
}

void PluginCallbacksManager::OnServerLibraryLoaded() {
    for (auto& module : modules_) {
        if (IsModuleEnabled(module->GetModuleName())) {
            try {
                module->OnServerLibraryLoaded();
            } catch (const std::exception& e) {
                std::cerr << "Error in " << module->GetModuleName() << "::OnServerLibraryLoaded: " << e.what() << std::endl;
            }
        }
    }
}

void PluginCallbacksManager::OnClientLibraryLoaded() {
    for (auto& module : modules_) {
        if (IsModuleEnabled(module->GetModuleName())) {
            try {
                module->OnClientLibraryLoaded();
            } catch (const std::exception& e) {
                std::cerr << "Error in " << module->GetModuleName() << "::OnClientLibraryLoaded: " << e.what() << std::endl;
            }
        }
    }
}

void PluginCallbacksManager::OnFrameUpdate() {
    for (auto& module : modules_) {
        if (IsModuleEnabled(module->GetModuleName())) {
            try {
                module->OnFrameUpdate();
            } catch (const std::exception& e) {
                std::cerr << "Error in " << module->GetModuleName() << "::OnFrameUpdate: " << e.what() << std::endl;
            }
        }
    }
}

void PluginCallbacksManager::OnCreateMove(CUserCmd* cmd, bool& gameActive) {
    for (auto& module : modules_) {
        if (IsModuleEnabled(module->GetModuleName())) {
            try {
                module->OnCreateMove(cmd, gameActive);
            } catch (const std::exception& e) {
                std::cerr << "Error in " << module->GetModuleName() << "::OnCreateMove: " << e.what() << std::endl;
            }
        }
    }
}

void PluginCallbacksManager::OnSqvmCreated(void* c_sqvm) {
    for (auto& module : modules_) {
        if (IsModuleEnabled(module->GetModuleName())) {
            try {
                module->OnSqvmCreated(c_sqvm);
            } catch (const std::exception& e) {
                std::cerr << "Error in " << module->GetModuleName() << "::OnSqvmCreated: " << e.what() << std::endl;
            }
        }
    }
}

void PluginCallbacksManager::OnSqvmDestroying(void* c_sqvm) {
    for (auto& module : modules_) {
        if (IsModuleEnabled(module->GetModuleName())) {
            try {
                module->OnSqvmDestroying(c_sqvm);
            } catch (const std::exception& e) {
                std::cerr << "Error in " << module->GetModuleName() << "::OnSqvmDestroying: " << e.what() << std::endl;
            }
        }
    }
}

void PluginCallbacksManager::OnConfigChanged() {
    // Publish event for other systems
    EventSystem::GetInstance().Publish(EventSystem::Events::CONFIG_CHANGED);
    
    // Notify all modules
    for (auto& module : modules_) {
        try {
            module->OnConfigChanged();
        } catch (const std::exception& e) {
            std::cerr << "Error in " << module->GetModuleName() << "::OnConfigChanged: " << e.what() << std::endl;
        }
    }
}

void PluginCallbacksManager::ReloadAllConfigs() {
    extern Config g_config;
    g_config.reload();
    OnConfigChanged();
}

std::vector<std::string> PluginCallbacksManager::GetModuleNames() const {
    std::vector<std::string> names;
    names.reserve(modules_.size());
    
    for (const auto& module : modules_) {
        names.emplace_back(module->GetModuleName());
    }
    
    return names;
}

void PluginCallbacksManager::PrintModuleStatus() const {
    std::cout << "=== Plugin Module Status ===" << std::endl;
    std::cout << "Total modules: " << modules_.size() << std::endl;
    
    for (const auto& module : modules_) {
        const std::string name = module->GetModuleName();
        const bool enabled = IsModuleEnabled(name);
        const bool moduleEnabled = module->IsEnabled();
        
        std::cout << "  " << name << ": " 
                  << (enabled ? "ENABLED" : "DISABLED")
                  << " (Module: " << (moduleEnabled ? "ON" : "OFF") << ")" 
                  << std::endl;
    }
    std::cout << "===========================" << std::endl;
}

void PluginCallbacksManager::EnableModule(const std::string& moduleName) {
    module_enabled_state_[moduleName] = true;
    std::cout << "Enabled module: " << moduleName << std::endl;
}

void PluginCallbacksManager::DisableModule(const std::string& moduleName) {
    module_enabled_state_[moduleName] = false;
    std::cout << "Disabled module: " << moduleName << std::endl;
}

bool PluginCallbacksManager::IsModuleEnabled(const std::string& moduleName) const {
    auto it = module_enabled_state_.find(moduleName);
    return (it != module_enabled_state_.end()) ? it->second : true; // Default to enabled
}

void PluginCallbacksManager::InitializeEventHandlers() {
    auto& eventSystem = EventSystem::GetInstance();
    
    // Handle config changes
    eventSystem.Subscribe(EventSystem::Events::CONFIG_CHANGED, 
        [this](const std::string& /*eventId*/, void* /*data*/) {
            // Config was changed externally, notify modules
            for (auto& module : modules_) {
                try {
                    module->OnConfigChanged();
                } catch (const std::exception& e) {
                    std::cerr << "Error notifying " << module->GetModuleName() 
                              << " of config change: " << e.what() << std::endl;
                }
            }
        });
}
