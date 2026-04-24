#include "ExampleModule.h"
#include "../Config.h"
#include "../core/EventSystem.h"
#include <iostream>

void ExampleModule::Initialize() {
    std::cout << "ExampleModule: Initializing..." << std::endl;
    
    last_action_time_ = std::chrono::steady_clock::now();
    action_count_ = 0;
    
    // Load configuration
    OnConfigChanged();
    
    // Subscribe to events
    auto& eventSystem = EventSystem::GetInstance();
    
    // Example: React to config changes
    eventSystem.Subscribe(EventSystem::Events::CONFIG_CHANGED, 
        [this](const std::string& eventId, void* data) {
            std::cout << "ExampleModule: Received config change event" << std::endl;
        });
    
    // Example: React to player connections
    eventSystem.Subscribe(EventSystem::Events::PLAYER_CONNECTED, 
        [this](const std::string& eventId, void* data) {
            if (data) {
                std::string* playerInfo = static_cast<std::string*>(data);
                std::cout << "ExampleModule: Player connected - " << *playerInfo << std::endl;
            }
        });
    
    if (enabled_) {
        std::cout << "ExampleModule: Initialized successfully" << std::endl;
    } else {
        std::cout << "ExampleModule: Initialized but disabled by config" << std::endl;
    }
}

void ExampleModule::Shutdown() {
    std::cout << "ExampleModule: Shutting down..." << std::endl;
    std::cout << "ExampleModule: Performed " << action_count_ << " actions during runtime" << std::endl;
}

void ExampleModule::OnFrameUpdate() {
    if (!enabled_) return;
    
    // Example: Do something every 5 seconds
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_action_time_).count();
    
    if (elapsed >= 5) {
        DoSomething();
        last_action_time_ = now;
    }
}

void ExampleModule::OnClientLibraryLoaded() {
    if (enabled_) {
        std::cout << "ExampleModule: Client library loaded, ready for client-side operations" << std::endl;
        PublishExampleEvent();
    }
}

void ExampleModule::OnConfigChanged() {
    extern Config g_config;
    
    // Example: Read a custom config value
    // You would need to add this to your Config class
    // enabled_ = g_config.getBool("example_module_enabled");
    
    // For now, just use debug logging as a proxy
    bool wasEnabled = enabled_;
    enabled_ = g_config.isDebugLoggingEnabled(); // Just for demonstration
    
    if (enabled_ != wasEnabled) {
        std::cout << "ExampleModule: Status changed to " 
                  << (enabled_ ? "ENABLED" : "DISABLED") << std::endl;
    }
}

void ExampleModule::DoSomething() {
    if (!enabled_) return;
    
    action_count_++;
    std::cout << "ExampleModule: Performing action #" << action_count_ << std::endl;
    
    // Example: Publish a custom event
    std::string actionData = "Action " + std::to_string(action_count_);
    EventSystem::GetInstance().Publish("example_action", &actionData);
}

void ExampleModule::PublishExampleEvent() {
    std::string eventData = "Example module is ready!";
    EventSystem::GetInstance().Publish("example_ready", &eventData);
    std::cout << "ExampleModule: Published ready event" << std::endl;
}
