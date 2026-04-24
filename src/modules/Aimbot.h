#pragma once

#include "../core/IFeatureModule.h"
#include "../pch.h"

class Aimbot : public IFeatureModule {
public:
  Aimbot() = default;
  virtual ~Aimbot() = default;

  void Initialize() override;
  void Shutdown() override;
  void OnFrameUpdate() override;
  void OnCreateMove(CUserCmd *cmd, bool &game_active) override;
  void OnConfigChanged() override;

  const char *GetModuleName() const override { return "Aimbot"; }
  bool IsEnabled() const override { return enabled_; }
  void SetEnabled(bool enabled) override { enabled_ = enabled; }

private:
  bool enabled_ = false;

  bool IsVisible(Vector3 bone_pos, CBaseEntity *local, CBaseEntity *entity);
};  