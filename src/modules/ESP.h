#pragma once

#include "../core/IFeatureModule.h"
#include "../utilities/Types.h"

#define METERS_TO_INCHES 39.3701f
#define INCHES_TO_METERS 0.0254f

class ESP : public IFeatureModule {
public:
  ESP() = default;
  virtual ~ESP() = default;

  // IFeatureModule interface
  void Initialize() override;
  void Shutdown() override;
  void OnFrameUpdate() override;
  void OnConfigChanged() override;

  // Draw ESP from an external paint callback (e.g. PaintTraverse).
  static void DrawOverlayPass();

  const char *GetModuleName() const override { return "ESP"; }
  bool IsEnabled() const override { return enabled_; }
  void SetEnabled(bool enabled) override { enabled_ = enabled; }

private:
  bool enabled_ = false;

  void LogESPInfo(const std::string& message, const std::string& context = "ESP");

  bool WorldToScreen(Vector3 src, Vector3 &dst);
  bool WorldToScreenWithBoundCheck(Vector3 &entpos);
  bool IsVisible(Vector3 start, Vector3 end, CBaseEntity *local,
                 CBaseEntity *entity);
  void RenderTracer(CBaseEntity *entity, const Color &color);
  void RenderBoundingBox(CBaseEntity* local, CBaseEntity* entity, int entnum);

  static const VMatrix *GetViewMatrix();

  bool GetPlayer2DBounds(CBaseEntity *entity, Vector4 &out);
  bool GetTitan2DBounds(CBaseEntity *entity, Vector4 &out);
};