#pragma once

#include "../pch.h"
#include "../core/IFeatureModule.h"

class Speed : public IFeatureModule {
public:
Speed() = default;
virtual ~Speed() = default;

    void Initialize() override;
    void Shutdown() override;
    void OnFrameUpdate() override;
    void OnCreateMove(CUserCmd* cmd, bool& game_active) override;
    void OnConfigChanged() override;

    const char *GetModuleName() const override { return "Speed"; }
    bool IsEnabled() const override { return enabled_; }
    void SetEnabled(bool enabled) override { enabled_ = enabled; }

private:
  bool enabled_ = false;
};