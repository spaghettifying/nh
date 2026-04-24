#include "DebugModule.h"
#include "../Config.h"
#include "../CppPluginExample.h"
#include <iostream>

void DebugModule::Initialize() {
    std::cout << "DebugModule: Initializing..." << std::endl;
    
    OnConfigChanged();
    
    if (enabled_) {
        LogDebugInfo("Debug module initialized", "INIT");
    }
}

void DebugModule::Shutdown() {
    if (enabled_) {
        LogDebugInfo("Debug module shutting down", "SHUTDOWN");
    }
    
    std::cout << "DebugModule: Shutdown complete" << std::endl;
}

void DebugModule::OnServerLibraryLoaded() {
    if (enabled_) {
        LogDebugInfo("Server library loaded", "SERVER");
        
        // Log server-specific information
        if (g_nssys) {
            g_nssys->Log(g_handle, LogLevel::INFO, "DebugModule: Server.dll loaded and monitored");
        }
    }
}

void DebugModule::OnClientLibraryLoaded() {
    if (enabled_) {
        LogDebugInfo("Client library loaded", "CLIENT");
        
        // Log client-specific information
        if (g_nssys) {
            g_nssys->Log(g_handle, LogLevel::INFO, "DebugModule: Client.dll loaded and monitored");
        }
    }
}

void DebugModule::OnSqvmCreated(void* c_sqvm) {
    if (enabled_) {
        LogDebugInfo("Squirrel VM created", "SQVM");
        std::cout << "DebugModule: SQVM created at address: " << c_sqvm << std::endl;
    }
}

void DebugModule::OnSqvmDestroying(void* c_sqvm) {
    if (enabled_) {
        LogDebugInfo("Squirrel VM destroying", "SQVM");
        std::cout << "DebugModule: SQVM destroying at address: " << c_sqvm << std::endl;
    }
}

void DebugModule::OnConfigChanged() {
    extern Config g_config;
    
    bool wasEnabled = enabled_;
    enabled_ = g_config.isDebugLoggingEnabled();
    
    if (enabled_ && !wasEnabled) {
        LogDebugInfo("Debug logging enabled", "CONFIG");
    } else if (!enabled_ && wasEnabled) {
        LogDebugInfo("Debug logging disabled", "CONFIG");
    }
}

void DebugModule::LogDebugInfo(const std::string& message, const std::string& context) {
    if (!enabled_) return;
    
    std::cout << "[DEBUG";
    if (!context.empty()) {
        std::cout << ":" << context;
    }
    std::cout << "] " << message << std::endl;
    
    // Also log to Northstar system if available
    if (g_nssys && g_handle) {
        std::string fullMessage = "[" + context + "] " + message;
        g_nssys->Log(g_handle, LogLevel::INFO, fullMessage.c_str());
    }
}
