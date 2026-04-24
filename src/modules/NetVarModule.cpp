#include "NetVarModule.h"
#include "../Config.h"
#include "../NetVarManager.h"
#include <iostream>

void NetVarModule::Initialize() {
    std::cout << "NetVarModule: Initializing..." << std::endl;
    
    OnConfigChanged();
    
    if (enabled_) {
        std::cout << "NetVarModule: NetVar management enabled" << std::endl;
    }
}

void NetVarModule::Shutdown() {
    if (enabled_) {
        std::cout << "NetVarModule: Shutting down NetVar management" << std::endl;
    }
    
    std::cout << "NetVarModule: Shutdown complete" << std::endl;
}

void NetVarModule::OnClientLibraryLoaded() {
    if (enabled_ && dump_on_client_load_) {
        std::cout << "NetVarModule: Client loaded, dumping NetVars" << std::endl;
        DumpNetVars();
    }
}

void NetVarModule::OnConfigChanged() {
    extern Config g_config;
    
    enabled_ = g_config.isNetVarDumpEnabled();
    
    std::cout << "NetVarModule: Configuration updated - Enabled: " 
              << (enabled_ ? "Yes" : "No") << std::endl;
}

void NetVarModule::DumpNetVars() {
    if (!enabled_) {
        std::cout << "NetVarModule: NetVar dumping is disabled" << std::endl;
        return;
    }
    
    std::cout << "NetVarModule: Starting NetVar dump..." << std::endl;
    
    try {
        NetVarManager::Dump();
        std::cout << "NetVarModule: NetVar dump completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "NetVarModule: Error during NetVar dump: " << e.what() << std::endl;
    }
}

void NetVarModule::RefreshNetVars() {
    if (!enabled_) {
        std::cout << "NetVarModule: NetVar refresh is disabled" << std::endl;
        return;
    }
    
    std::cout << "NetVarModule: Refreshing NetVars..." << std::endl;
    
    // This would refresh/reload NetVar information
    // Implementation depends on your NetVarManager capabilities
    try {
        // NetVarManager::Refresh(); // If you have such a method
        DumpNetVars(); // For now, just re-dump
        std::cout << "NetVarModule: NetVar refresh completed" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "NetVarModule: Error during NetVar refresh: " << e.what() << std::endl;
    }
}
