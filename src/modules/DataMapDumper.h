#pragma once

#include "../core/IFeatureModule.h"
#include "../pch.h"

class DataMapDumper : public IFeatureModule {
public:
  DataMapDumper() = default;
  virtual ~DataMapDumper() = default;

  void Initialize() override;
  void Shutdown() override;
  void OnFrameUpdate() override;
  void OnConfigChanged() override;
  void OnClientLibraryLoaded() override;

  const char *GetModuleName() const override { return "DataMapDumper"; }
  bool IsEnabled() const override { return enabled_; }
  void SetEnabled(bool enabled) override { 
    enabled_ = enabled;
    DumpDataMap(10);
  }

private:
  bool enabled_ = false;
  bool dump_on_client_load_ = true;

  void DumpDataMap(int depth);
  void DumpDataMap(const datamap_t& map, int depth);
};