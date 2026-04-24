#include "ESP.h"

#include "../Config.h"
#include "../pch.h"
#include "../r2sdk/classes/VMatrix.h"
#include "../r2sdk/interfaces/IViewRender.h"
#include "../renderer/renderer.h"
#include <iostream>
#include <cfloat>

const VMatrix *ESP::GetViewMatrix() {
  // Prefer the engine-provided matrix to avoid transient pointer chains.
  if (g_Interfaces.EngineClient) {
    const auto &engineMatrix = g_Interfaces.EngineClient->WorldToScreenMatrix();
    return &engineMatrix;
  }

  return nullptr;
}

void ESP::Initialize() {
  if (g_nssys && g_handle) {
    g_nssys->Log(g_handle, LogLevel::INFO, "ESP: Initializing");
  }

  OnConfigChanged();

  if (enabled_) {
    LogESPInfo("ESP module initialized", "INIT");
  }
}

void ESP::Shutdown() {
  if (enabled_) {
    LogESPInfo("ESP module shutting down", "SHUTDOWN");
  }
}

void ESP::OnFrameUpdate() {
  // DrawOverlayPass();
}

void ESP::DrawOverlayPass() {
  if (!g_config.isESPEnabled()) {
    return;
  }
  ESP esp;
  esp.SetEnabled(true);

  if (!g_Interfaces.EngineClient || !g_Interfaces.ClientEntityList ||
      !MatSystemSurface) {
    esp.LogESPInfo("yeah matsurface oder interfaces");
    return;
  }

  if (!g_Interfaces.EngineClient->IsInGame()) {
    return;
  }

  // esp.LogESPInfo("before renderer get size");
  int w = 0;
  int h = 0;
  if (!Renderer::GetScreenSize(w, h)) {
    esp.LogESPInfo("renderer screensize fucked up");
    return;
  }

  CBaseEntity *local = reinterpret_cast<CBaseEntity *>(
      g_Interfaces.ClientEntityList->GetClientEntity(
          g_Interfaces.EngineClient->GetLocalPlayer()));

  const int localTeam = local->GetTeamNum();
  const Vector3 localOrigin = local->GetLocalOrigin();

  if (!local || !local->IsAlive()) {
    esp.LogESPInfo(fmt::format("local player wasnt alive or sum, local? {}",
                               local == nullptr));
    return;
  }

  Renderer::DrawFilledRect(w / 2, h / 2, 3, 3, Color(0, 255, 0, 255));

  const int highest = g_Interfaces.ClientEntityList->GetMaxEntities();
  for (int i = 0; i < highest; i++) {
    IClientEntity *clientEnt =
        g_Interfaces.ClientEntityList->GetClientEntity(i);
    if (local == clientEnt)
      continue;

    if (clientEnt == nullptr) {
      continue;
    }

    CBaseEntity *ent = reinterpret_cast<CBaseEntity *>(clientEnt);

    if (ent == nullptr)
      continue;

    const char *signame = ent->GetSignifierName();
    if (signame == nullptr)
      continue;

    if (localTeam == ent->GetTeamNum())
      continue;

    if (ent->IsPlayerDecoy())
      continue;

    if (!(*signame == 'p' && *(signame + 1) == 'l' && ent->GetLifeState() == 0))
      continue;

    const Vector3 entOrigin = ent->GetLocalOrigin();
    const Vector3 dir = entOrigin - localOrigin;
    const float dist =
        std::sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z) * .1f;

    esp.RenderTracer(ent, Color(255, 255, 0, 220));
    esp.RenderBoundingBox(local, ent, i);
  }
}

void ESP::RenderTracer(CBaseEntity *entity, const Color &color) {
  if (!entity) {
    return;
  }

  int w = 0;
  int h = 0;
  if (!Renderer::GetScreenSize(w, h) || w <= 0 || h <= 0) {
    return;
  }

  const Vector3 origin = entity->GetLocalOrigin();
  Vector3 footWorld{origin.x, origin.y, origin.z + 6.0f};
  Vector3 footScreen{};
  if (!WorldToScreen(footWorld, footScreen)) {
    return;
  }

  const int startX = w / 2;
  const int startY = h - 2;
  Renderer::DrawLine(startX, startY, static_cast<int>(footScreen.x),
                     static_cast<int>(footScreen.y), color);
}

void ESP::OnConfigChanged() { enabled_ = g_config.isESPEnabled(); }

bool ESP::WorldToScreen(Vector3 src, Vector3 &dst) {
  if (!std::isfinite(src.x) || !std::isfinite(src.y) || !std::isfinite(src.z)) {
    return false;
  }

  if (!g_Interfaces.EngineClient) {
    return false;
  }

  if (!ViewRenderPtr) {
    return false;
  }

  auto *const viewRender = *reinterpret_cast<IViewRender **>(ViewRenderPtr);
  if (!viewRender) {
    return false;
  }

  const Matrix4 *worldToScreen = nullptr;
  if (viewRender->HasWorldToScreenMatrix(false)) {
    worldToScreen = viewRender->GetWorldToScreenMatrix(false);
  }
  if (!worldToScreen && viewRender->HasWorldToScreenMatrix(true)) {
    worldToScreen = viewRender->GetWorldToScreenMatrix(true);
  }

  if (!worldToScreen) {
    return false;
  }

  // Copy matrix by value so we never dereference engine-owned memory after this
  // point.
  const Matrix4 mat = *worldToScreen;

  for (int r = 0; r < 4; ++r) {
    for (int c = 0; c < 4; ++c) {
      if (!std::isfinite(mat.m[r][c])) {
        return false;
      }
    }
  }

  // Snapshot rows once so we don't repeatedly index engine-backed memory.
  const float m00 = mat.m[0][0], m01 = mat.m[0][1], m02 = mat.m[0][2],
              m03 = mat.m[0][3];
  const float m10 = mat.m[1][0], m11 = mat.m[1][1], m12 = mat.m[1][2],
              m13 = mat.m[1][3];
  const float m30 = mat.m[3][0], m31 = mat.m[3][1], m32 = mat.m[3][2],
              m33 = mat.m[3][3];

  float w = m30 * src.x + m31 * src.y + m32 * src.z + m33;
  if (!std::isfinite(w) || w < 0.01f) {
    return false;
  }

  float x = m00 * src.x + m01 * src.y + m02 * src.z + m03;
  float y = m10 * src.x + m11 * src.y + m12 * src.z + m13;
  if (!std::isfinite(x) || !std::isfinite(y)) {
    return false;
  }

  const float invW = 1.0f / w;
  x *= invW;
  y *= invW;

  // Reject unstable projection results before mapping to pixels.
  if (!std::isfinite(x) || !std::isfinite(y) || std::fabs(x) > 100.0f ||
      std::fabs(y) > 100.0f) {
    return false;
  }

  int width = 0;
  int height = 0;
  if (!Renderer::GetScreenSize(width, height) || width <= 0 || height <= 0) {
    return false;
  }

  if (!std::isfinite(x) || !std::isfinite(y)) {
    return false;
  }

  dst.x = (width * 0.5f) + (x * width * 0.5f);
  dst.y = (height * 0.5f) - (y * height * 0.5f);
  if (!std::isfinite(dst.x) || !std::isfinite(dst.y)) {
    return false;
  }

  // Avoid undefined float->int conversions downstream.
  if (std::fabs(dst.x) > 1'000'000.0f || std::fabs(dst.y) > 1'000'000.0f) {
    return false;
  }

  dst.z = 0.0f;
  return true;
}

bool ESP::IsVisible(Vector3 start, Vector3 end, CBaseEntity *local,
                    CBaseEntity *entity) {
  if (!g_Interfaces.EngineTrace || !local || !entity) {
    return false;
  }

  Ray_t ray;
  ray.init(start, end);

  Trace_t trace{};

  CTraceFilter filter;
  filter.pSkipEntity = local;
  g_Interfaces.EngineTrace->TraceRay(ray, TRACE_MASK_SHOT, &filter, &trace);

  if (trace.allSolid || trace.startSolid) {
    return false;
  }

  // If most of the ray traveled unobstructed, treat target as visible.
  if (trace.fraction >= 0.97f) {
    return true;
  }

  auto *traceEnt = reinterpret_cast<CBaseEntity *>(trace.entity);
  if (!traceEnt || traceEnt == local) {
    return false;
  }

  if (traceEnt == entity) {
    return true;
  }

  return traceEnt->entindex() == entity->entindex();
}

void ESP::RenderBoundingBox(CBaseEntity *local, CBaseEntity *entity, int entnum) {
  const Vector3 localOrigin = local->GetLocalOrigin();
  const Vector3 entityOrigin = entity->GetLocalOrigin();

  if (!std::isfinite(entityOrigin.x) || !std::isfinite(entityOrigin.y) ||
      !std::isfinite(entityOrigin.z)) {
    return;
  }

  Vector4 bounds{};
  const bool gotBounds = entity->IsTitan() ? GetTitan2DBounds(entity, bounds)
                                           : GetPlayer2DBounds(entity, bounds);
  if (!gotBounds) {
    return;
  }

  if (!std::isfinite(bounds.x) || !std::isfinite(bounds.y) ||
      !std::isfinite(bounds.z) || !std::isfinite(bounds.w)) {
    return;
  }

  const float boxWf = bounds.z;
  const float boxHf = bounds.w;
  const float boxXf = bounds.x - (boxWf * 0.5f);
  const float boxYf = bounds.y - boxHf;
  if (!std::isfinite(boxXf) || !std::isfinite(boxYf) ||
      !std::isfinite(boxWf) || !std::isfinite(boxHf)) {
    return;
  }

  const int x = static_cast<int>(boxXf);
  const int y = static_cast<int>(boxYf);
  const int w = static_cast<int>(boxWf);
  const int h = static_cast<int>(boxHf);
  if (w <= 0 || h <= 0 || w > 4000 || h > 4000)
    return;

  const bool playerVisible =
      IsVisible(localOrigin, entityOrigin, local, entity);
  const Color visibleColor =
      playerVisible ? Color(0, 255, 0, 255) : Color(255, 0, 0, 255);

  const int t = 5;
  Renderer::DrawOutlinedRect(x, y, w, h, visibleColor);
  Renderer::DrawOutlinedRect(x - t, y - t, w + (t * 2), h + (t * 2),
                             Color(0, 0, 0, 200));
  Renderer::DrawOutlinedRect(x + t, y + t, w - (t * 2), h - (t * 2),
                             Color(0, 0, 0, 200));

  player_info_t player_info{};
  char *entName = "player";
  if (g_Interfaces.EngineClient->GetPlayerInfo(entnum, &player_info))
    entName = player_info.name;

  Renderer::RDrawText(5, x + w + 2, y + 6, Color(255, 255, 255, 255), entName);
  Renderer::RDrawText(5, x + w + 2, y + 12, Color(255, 255, 255, 255), fmt::format("Health: {}", entity->GetHealth()).c_str());
}

void ESP::LogESPInfo(const std::string &message, const std::string &context) {
  if (!enabled_) {
    return;
  }

  std::cout << "[ESP";
  if (!context.empty()) {
    std::cout << ":" << context;
  }
  std::cout << "] " << message << std::endl;

  if (g_nssys && g_handle) {
    std::string fullMessage = "[" + context + "] " + message;
    g_nssys->Log(g_handle, LogLevel::INFO, fullMessage.c_str());
  }
}

bool ESP::GetPlayer2DBounds(CBaseEntity* entity, Vector4& out) {
  Vector3 entOrigin = entity->GetLocalOrigin();
  Vector3 entHead = entOrigin - Vector3{0, 0, -70};
 
  if (!WorldToScreen(entOrigin, entOrigin) || !WorldToScreen(entHead, entHead))
    return false;

  const float entHeight = entOrigin.y - entHead.y;
  const float entWidth = entHeight * .5f;

  out = Vector4{entOrigin.x, entOrigin.y, entWidth, entHeight};

  return true;
}

bool ESP::GetTitan2DBounds(CBaseEntity* entity, Vector4& out) {
  Vector3 entOrigin = entity->GetLocalOrigin();
  Vector3 entHead = entOrigin - Vector3{0, 0, -216};
 
  if (!WorldToScreen(entOrigin, entOrigin) || !WorldToScreen(entHead, entHead))
    return false;

  const float entHeight = entOrigin.y - entHead.y;
  const float entWidth = entHeight * .5f;

  out = Vector4{entOrigin.x, entOrigin.y, entWidth, entHeight};

  return true;
}