#include "Speed.h"

void Speed::Initialize() {
    if (g_nssys && g_handle)
      g_nssys->Log(g_handle, LogLevel::INFO, "Speed: Initializing");

    OnConfigChanged();

    if (enabled_)
      g_nssys->Log(g_handle, LogLevel::INFO, "Speed initialized");
}

void Speed::Shutdown() {
    if (enabled_)
      g_nssys->Log(g_handle, LogLevel::INFO, "Speed shutting down");
}

void Speed::OnFrameUpdate() {
 // nop
}

void Speed::OnCreateMove(CUserCmd* cmd, bool& game_active) {
    if (enabled_ && InputManager::GetInstance().IsKeyPressed(KEY_RIGHT))
      cmd->m_gameTime += g_config.getFloat("speed");
}

void Speed::OnConfigChanged() {
  enabled_ = g_config.getBool("speed_enabled");
}

