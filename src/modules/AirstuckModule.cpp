#include "AirstuckModule.h"

void Airstuck::Initialize() {
    if (g_nssys && g_handle)
      g_nssys->Log(g_handle, LogLevel::INFO, "Airstuck: Initializing");

    OnConfigChanged();

    if (enabled_)
      g_nssys->Log(g_handle, LogLevel::INFO, "Airstuck initialized");
}

void Airstuck::Shutdown() {
    if (enabled_)
      g_nssys->Log(g_handle, LogLevel::INFO, "Airstuck shutting down");
}

void Airstuck::OnFrameUpdate() {
 // nop
}

void Airstuck::OnCreateMove(CUserCmd* cmd, bool& game_active) {
    if (enabled_ && InputManager::GetInstance().IsKeyPressed(KEY_LEFT))
      cmd->m_commandNumber = 0x7FFFFFFF;
}

void Airstuck::OnConfigChanged() {
  enabled_ = g_config.getBool("airstuck_enabled");
}

