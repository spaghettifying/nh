#include "DataMapDumper.h"
#include "../Config.h"
#include <iostream>

void DataMapDumper::Initialize() {
	std::cout << "DataMapDumper: Initializing..." << std::endl;
	OnConfigChanged();

	if (enabled_) {
		std::cout << "DataMapDumper: Enabled" << std::endl;
	} else {
		std::cout << "DataMapDumper: Disabled" << std::endl;
	}
}

void DataMapDumper::Shutdown() {
	if (enabled_) {
		std::cout << "DataMapDumper: Shutting down" << std::endl;
	}
	std::cout << "DataMapDumper: Shutdown complete" << std::endl;
}

void DataMapDumper::OnFrameUpdate() {
	// Intentionally left minimal. Integrate walker/calls elsewhere.
}

void DataMapDumper::OnConfigChanged() {
	// Read enabled flag from global config
	extern Config g_config;
	enabled_ = g_config.getBool("datamap_dump_enabled");

	if (enabled_)
          DumpDataMap(10);

    std::cout << "DataMapDumper: Configuration updated - Enabled: "
			  << (enabled_ ? "Yes" : "No") << std::endl;
}

void DataMapDumper::OnClientLibraryLoaded() {
    std::cout << "DataMapDumper: Client library loaded, ready for data map dumping" << std::endl;
}

void DataMapDumper::DumpDataMap(int depth) {
  std::cout << "DataMapDumper::DumpDataMap called" << std::endl;
  if (!g_Interfaces.ClientEntityList || !g_Interfaces.EngineClient) {
    std::cout << "DataMapDumper: Game interfaces not ready yet" << std::endl;
    return;
  }
  g_nssys->Log(g_handle, LogLevel::INFO, "Dumping datamap before local");
  CBaseEntity *local = reinterpret_cast<CBaseEntity *>(
      g_Interfaces.ClientEntityList->GetClientEntity(
          g_Interfaces.EngineClient->GetLocalPlayer()));
  g_nssys->Log(g_handle, LogLevel::INFO, "Dumping datamap after local");
  DumpDataMap(local->GetDataDescMap(), depth);
}

void DataMapDumper::DumpDataMap(const datamap_t& map, int depth) {
  g_nssys->Log(g_handle, LogLevel::INFO, "DUMPING DATAMAP");
  g_nssys->Log(g_handle, LogLevel::INFO, map.dataClassName);
}