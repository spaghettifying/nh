#include "CreateMoveHook.h"

#include "../core/PluginCallbacksManager.h"
#include "../input/InputManager.h"
#include "../utilities/PatternScanner.h"
#include <polyhook2/Detour/x64Detour.hpp>
#include "../pch.h"

namespace {
using CreateMoveFn = bool(__fastcall *)(void *, float, CUserCmd *, bool);

constexpr const char *kCreateMoveSig =
		"48 89 5C 24 ? 57 48 83 EC 30 48 8B 0D ? ? ? ? 0F 29 74 24";

uint64_t g_originalCreateMoveDetour = 0;
std::unique_ptr<PLH::x64Detour> g_createMoveDetour;
bool g_installed = false;
bool g_loggedFirstCall = false;
bool g_turnBindWasDown = false;

float NormalizeDeg(float angle) {
	while (angle > 180.0f) {
		angle -= 360.0f;
	}
	while (angle < -180.0f) {
		angle += 360.0f;
	}
	return angle;
}

bool __fastcall HookedCreateMove(void *pIClientMode,
																 float input_sample_frametime,
																 CUserCmd *pCUserCmd, bool game_active) {
	bool result = true;
	if (g_originalCreateMoveDetour) {
		auto original = reinterpret_cast<CreateMoveFn>(g_originalCreateMoveDetour);
		result = original(pIClientMode, input_sample_frametime, pCUserCmd,
											game_active);
	}

		if (!pCUserCmd)
			return result;

        // Keep hook lightweight: only prove the path once and avoid per-tick logs.
	if (!g_loggedFirstCall && g_nssys && g_handle) {
		const int cmdNum = pCUserCmd ? pCUserCmd->m_commandNumber : -1;
		g_nssys->Log(
				g_handle, LogLevel::INFO,
				fmt::format("CreateMove entered (cmd={}, active={})", cmdNum,
										game_active)
						.c_str());
		g_loggedFirstCall = true;
	}

		auto& manager = PluginCallbacksManager::GetInstance();
		manager.OnCreateMove(pCUserCmd, game_active);

		const bool turnBindDown = InputManager::GetInstance().IsKeyPressed(KEY_LALT);
		if (turnBindDown && !g_turnBindWasDown) {
			pCUserCmd->m_worldViewAngles.y = NormalizeDeg(pCUserCmd->m_worldViewAngles.y + 180.0f);
			pCUserCmd->m_localViewAngles.y = NormalizeDeg(pCUserCmd->m_localViewAngles.y + 180.0f);
		}
		g_turnBindWasDown = turnBindDown;

        return result;
}
} // namespace

bool CreateMoveHook::Initialize() {
	if (g_installed) {
		return true;
	}

	HMODULE client = GetModuleHandleA("client.dll");
	if (!client) {
		if (g_nssys && g_handle) {
			g_nssys->Log(g_handle, LogLevel::WARN,
									 "CreateMove hook init: client.dll not loaded");
		}
		return false;
	}

	const uintptr_t createMoveAddress = PatternScanner::FindPattern(client, kCreateMoveSig);
	if (!createMoveAddress) {
		if (g_nssys && g_handle) {
			g_nssys->Log(g_handle, LogLevel::WARN,
									 "CreateMove hook init: signature not found");
		}
		return false;
	}

	g_createMoveDetour = std::make_unique<PLH::x64Detour>(
			createMoveAddress, reinterpret_cast<uint64_t>(&HookedCreateMove),
			&g_originalCreateMoveDetour);

	if (!g_createMoveDetour->hook()) {
		g_createMoveDetour.reset();
		g_originalCreateMoveDetour = 0;
		if (g_nssys && g_handle) {
			g_nssys->Log(g_handle, LogLevel::WARN,
									 "CreateMove hook init: detour install failed");
		}
		return false;
	}

	g_installed = true;
	g_loggedFirstCall = false;
	if (g_nssys && g_handle) {
		g_nssys->Log(g_handle, LogLevel::INFO,
								 fmt::format("CreateMove detour installed at 0x{:X}",
														 createMoveAddress)
										 .c_str());
	}
	return true;
}

void CreateMoveHook::Shutdown() {
	if (g_createMoveDetour) {
		g_createMoveDetour->unHook();
		g_createMoveDetour.reset();
	}

	g_originalCreateMoveDetour = 0;
	g_installed = false;
	g_loggedFirstCall = false;
}

bool CreateMoveHook::IsInstalled() { return g_installed; }
