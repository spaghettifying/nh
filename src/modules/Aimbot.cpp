#include "Aimbot.h"

namespace {
float NormalizeYaw(float yaw) {
  while (yaw > 180.0f) {
    yaw -= 360.0f;
  }
  while (yaw < -180.0f) {
    yaw += 360.0f;
  }
  return yaw;
}

float ClampPitch(float pitch) {
  if (pitch > 89.0f) {
    return 89.0f;
  }
  if (pitch < -89.0f) {
    return -89.0f;
  }
  return pitch;
}

Vector3 CalcAimAngles(const Vector3& src, const Vector3& dst) {
  const Vector3 delta = dst - src;
  const float hyp = std::sqrtf(delta.x * delta.x + delta.y * delta.y);

  Vector3 ang{};
  ang.x = -std::atan2f(delta.z, hyp) * (180.0f / std::numbers::pi_v<float>);
  ang.y = std::atan2f(delta.y, delta.x) * (180.0f / std::numbers::pi_v<float>);
  ang.z = 0.0f;

  ang.x = ClampPitch(ang.x);
  ang.y = NormalizeYaw(ang.y);
  return ang;
}

Vector2 CalcAngleDelta2D(const Vector3& target, const Vector3& current) {
  const float pitchDelta = target.x - current.x;
  const float yawDelta = NormalizeYaw(target.y - current.y);
  return {pitchDelta, yawDelta};
}
} // namespace

void Aimbot::Initialize() {
    if (g_nssys && g_handle)
      g_nssys->Log(g_handle, LogLevel::INFO, "Aimbot: Initializing");

    OnConfigChanged();

    if (enabled_)
      g_nssys->Log(g_handle, LogLevel::INFO, "Aimbot initialized");
}

void Aimbot::Shutdown() {
    if (enabled_)
      g_nssys->Log(g_handle, LogLevel::INFO, "Aimbot shutting down");
}

void Aimbot::OnFrameUpdate() {
 // nop
}

void Aimbot::OnCreateMove(CUserCmd* cmd, bool& game_active) {
  if (!enabled_ || !cmd) {
    return;
  }

  if (!InputManager::GetInstance().IsKeyPressed(MOUSE_LEFT)) {
    return;
  }

  if (!g_Interfaces.EngineClient || !g_Interfaces.ClientEntityList) {
    g_nssys->Log(g_handle, LogLevel::INFO, "Aimbot: no interfaces");
    return;
  }

  if (!g_Interfaces.EngineClient->IsInGame()) {
    return;
  }

  CBaseEntity *local = reinterpret_cast<CBaseEntity *>(
      g_Interfaces.ClientEntityList->GetClientEntity(
          g_Interfaces.EngineClient->GetLocalPlayer()));

  if (!local || !local->IsAlive()) {
    return;
  }

  const int localTeam = local->GetTeamNum();
  const Vector3 localEye = local->GetCameraPos();
  const float maxFov = std::max(0.1f, g_config.getFloat("aimbot_fov"));
  const Vector3 currentAngles = cmd->m_worldViewAngles;

  bool foundTarget = false;
  Vector3 bestAngles{};
  int bestHealth = 0x7fffffff;
  float bestFov = maxFov;

  const int highest = g_Interfaces.ClientEntityList->GetMaxEntities();
  for (int i = 0; i < highest; i++) {
    IClientEntity *clientEnt =
        g_Interfaces.ClientEntityList->GetClientEntity(i);

    if (local == clientEnt)
      continue;

    if (clientEnt == nullptr)
      continue;

    CBaseEntity *ent = reinterpret_cast<CBaseEntity *>(clientEnt);
    if (ent == nullptr)
      continue;

    matrix3x4_t boneMatrix[256]{};
    if (!ent->SetupBones(&boneMatrix[0], 256, 1024, g_pGlobalVars->curtime)) {
        continue;
    }

    constexpr int head_bone_id = 12;
    Vector3 bone_pos =
        Vector3{boneMatrix[head_bone_id][0][3], boneMatrix[head_bone_id][1][3],
                boneMatrix[head_bone_id][2][3]};

    if (!IsVisible(bone_pos, local, ent)) {
      continue;
    }

    const char *signame = ent->GetSignifierName();
    if (signame == nullptr)
      continue;

    if (localTeam == ent->GetTeamNum())
      continue;

    if (ent->IsPlayerDecoy())
      continue;

    if (!(*signame == 'p' && *(signame + 1) == 'l' && ent->GetLifeState() == 0))
      continue;

    const int health = ent->GetHealth();
    if (health <= 0)
      continue;

    const Vector2 aimAngles =
      Math::CalcAngles(localEye, bone_pos, -89, 89, -180, 180,
               local->GetWeaponSway());
    const Vector3 targetAngles{aimAngles.y, aimAngles.x, 0.0f};
    const Vector2 delta2D = CalcAngleDelta2D(targetAngles, currentAngles);
    const float fov = std::sqrtf(delta2D.x * delta2D.x + delta2D.y * delta2D.y);
    if (fov > maxFov)
      continue;

    if (health < bestHealth || (health == bestHealth && fov < bestFov)) {
      bestHealth = health;
      bestFov = fov;
      bestAngles = targetAngles;
      foundTarget = true;
    }
  }

  if (!foundTarget) {
    return;
  }

  Vector3 out = bestAngles;

  cmd->m_attackAngles = out;

  if (g_config.getBool("aimbot_autoshoot")) {
    cmd->m_buttons |= 1;
  }
}

void Aimbot::OnConfigChanged() {
  enabled_ = g_config.getBool("aimbot_enabled");
}

bool Aimbot::IsVisible(Vector3 bone_pos, CBaseEntity* local, CBaseEntity* entity) {
  IEngineTrace *pIEngineTrace = g_Interfaces.EngineTrace;
  if (!pIEngineTrace || !local || !entity) {
    return false;
  }

  Trace_t trace{};
  Ray_t ray{};

  CTraceFilter trace_filter{};
  trace_filter.pSkipEntity = local;

  Vector3 start = local->GetCameraPos();

  ray.init(start, bone_pos);

  pIEngineTrace->TraceRay(ray, TRACE_MASK_SHOT, &trace_filter, &trace);

  if (trace.allSolid || trace.startSolid) {
    return false;
  }

  // If most of the ray traveled unobstructed, treat target as visible.
  if (trace.fraction >= 0.97f) {
    return true;
  }

  auto *traceEnt = reinterpret_cast<CBaseEntity *>(trace.entity);
  if (!traceEnt || traceEnt == local)
    return false;

  if (traceEnt == entity) {
    return true;
  }

  return traceEnt->entindex() == entity->entindex();
}