#include "PaintTraverseHook.h"

#include "../Config.h"
#include "../core/PluginCallbacksManager.h"
#include "../modules/ESP.h"
#include "../renderer/renderer.h"
#include "../utilities/PatternScanner.h"
#include <polyhook2/Detour/x64Detour.hpp>

namespace {
using PaintTraverseFn = void(__thiscall *)(void *, unsigned int, bool, bool);
using PaintTraverseDetourFn =
    void(__fastcall *)(void *, unsigned int, bool, bool);

constexpr int kCandidatePaintTraverseIndices[] = {41, 42, 40, 43};
constexpr const char *kPaintTraverseSig =
    "48 89 5C 24 ? 57 48 83 EC 20 48 8B 01 41 0F B6 D9 41 0F B6 F8 FF 90";

PaintTraverseFn g_originalPaintTraverse = nullptr;
uint64_t g_originalPaintTraverseDetour = 0;
std::unique_ptr<PLH::x64Detour> g_paintTraverseDetour;
void **g_panelVtable = nullptr;
bool g_installed = false;
int g_activePaintTraverseIndex = -1;
bool g_loggedPaintTraverseEntry = false;

void DrawFromPaintTraverse(unsigned int panel, const char *sourceTag) {
  if (!MatSystemSurface) {
    return;
  }

  const uintptr_t embeddedPanelRaw = MatSystemSurface->GetEmbeddedPanel();
  const unsigned int embeddedPanel = static_cast<unsigned int>(embeddedPanelRaw);
  if (!embeddedPanel || panel != embeddedPanel) {
    return;
  }

  if (!g_loggedPaintTraverseEntry && g_nssys && g_handle) {
    g_nssys->Log(
        g_handle, LogLevel::INFO,
        fmt::format("{} entered: panel={} embedded={}",
                    sourceTag ? sourceTag : "PaintTraverse", panel, embeddedPanel)
            .c_str());
    g_loggedPaintTraverseEntry = true;
  }

  MatSystemSurface->PushMakeCurrent(panel, true);
  MatSystemSurface->DrawColoredTextWrapper(5, 2, 2, 255, 255, 255, 255,
                                           g_config.getString("title").c_str());

  if (g_config.isDisplayModuleListEnabled()) {
    auto &manager = PluginCallbacksManager::GetInstance();
    const auto moduleNames = manager.GetModuleNames();

    int y = 18;
    Renderer::RDrawText(5, 2, y, Color(255, 255, 255, 255), "Modules:");
    y += 12;

    for (const auto &name : moduleNames) {
      const bool enabled = manager.IsModuleEnabled(name);
      if (!enabled) {
        continue;
      }

      Renderer::RDrawText(5, 2, y, Color(120, 255, 120, 255), name.c_str());
      y += 12;
    }
  }

  if (g_config.isRendererSelfTestEnabled()) {
    Renderer::DrawSelfTestOverlay();
  }

  ESP::DrawOverlayPass();
  MatSystemSurface->PopMakeCurrent(panel);
}

bool PatchVTableEntry(void **vtable, int index, void *replacement,
                      void **originalOut) {
  if (!vtable || !replacement || !originalOut) {
    return false;
  }

  DWORD oldProtect = 0;
  if (!VirtualProtect(&vtable[index], sizeof(void *), PAGE_EXECUTE_READWRITE,
                      &oldProtect)) {
    return false;
  }

  *originalOut = vtable[index];
  vtable[index] = replacement;

  DWORD unused = 0;
  VirtualProtect(&vtable[index], sizeof(void *), oldProtect, &unused);
  return true;
}

void __fastcall HookedPaintTraverse(void *thisptr, void *, unsigned int panel,
                                    bool forceRepaint, bool allowForce) {
  if (g_originalPaintTraverse) {
    g_originalPaintTraverse(thisptr, panel, forceRepaint, allowForce);
  }

  DrawFromPaintTraverse(panel, "HookedPaintTraverse(vtable)");
}

void __fastcall HookedPaintTraverseDetour(void *thisptr, unsigned int panel,
                                          bool forceRepaint,
                                          bool allowForce) {
  if (g_originalPaintTraverseDetour) {
    auto original =
        reinterpret_cast<PaintTraverseDetourFn>(g_originalPaintTraverseDetour);
    original(thisptr, panel, forceRepaint, allowForce);
  }

  DrawFromPaintTraverse(panel, "HookedPaintTraverse(detour)");
}
} // namespace

bool PaintTraverseHook::Initialize() {
  if (g_installed) {
    return true;
  }

  if (!VGuiPanel) {
    if (g_nssys && g_handle) {
      g_nssys->Log(g_handle, LogLevel::WARN,
                   "PaintTraverse hook init: VGuiPanel is null");
    }
    return false;
  }

  g_panelVtable = *reinterpret_cast<void ***>(VGuiPanel);
  if (!g_panelVtable) {
    if (g_nssys && g_handle) {
      g_nssys->Log(g_handle, LogLevel::WARN,
                   "PaintTraverse hook init: panel vtable is null");
    }
    return false;
  }

  bool vtableInstalled = false;
  for (const int index : kCandidatePaintTraverseIndices) {
    void *original = nullptr;
    if (!PatchVTableEntry(g_panelVtable, index,
                          reinterpret_cast<void *>(&HookedPaintTraverse),
                          &original)) {
      continue;
    }

    g_originalPaintTraverse = reinterpret_cast<PaintTraverseFn>(original);
    if (g_originalPaintTraverse) {
      g_activePaintTraverseIndex = index;
      vtableInstalled = true;
      if (g_nssys && g_handle) {
        g_nssys->Log(g_handle, LogLevel::INFO,
                     fmt::format("PaintTraverse hook installed at vtable index {}",
                                 index)
                         .c_str());
      }
      break;
    }

    DWORD oldProtect = 0;
    if (VirtualProtect(&g_panelVtable[index], sizeof(void *),
                       PAGE_EXECUTE_READWRITE, &oldProtect)) {
      g_panelVtable[index] = original;
      DWORD unused = 0;
      VirtualProtect(&g_panelVtable[index], sizeof(void *), oldProtect,
                     &unused);
    }
  }

  if (!vtableInstalled && g_nssys && g_handle) {
    g_nssys->Log(g_handle, LogLevel::WARN,
                 "PaintTraverse hook init failed for indices 41/42/40/43");
  }

  bool detourInstalled = false;
  HMODULE vgui2 = GetModuleHandleA("vgui2.dll");
  if (vgui2) {
    const uintptr_t paintTraverseAddress =
        PatternScanner::FindPattern(vgui2, kPaintTraverseSig);
    if (paintTraverseAddress) {
      g_paintTraverseDetour = std::make_unique<PLH::x64Detour>(
          paintTraverseAddress,
          reinterpret_cast<uint64_t>(&HookedPaintTraverseDetour),
          &g_originalPaintTraverseDetour);

      if (g_paintTraverseDetour->hook()) {
        detourInstalled = true;
        if (g_nssys && g_handle) {
          g_nssys->Log(g_handle, LogLevel::INFO,
                       fmt::format("PaintTraverse detour installed at 0x{:X}",
                                   paintTraverseAddress)
                           .c_str());
        }
      } else {
        g_paintTraverseDetour.reset();
      }
    }
  }

  if (!detourInstalled && g_nssys && g_handle) {
    g_nssys->Log(g_handle, LogLevel::WARN,
                 "PaintTraverse detour fallback was not installed");
  }

  g_installed = vtableInstalled || detourInstalled;
  return g_installed;
}

void PaintTraverseHook::Shutdown() {
  if (g_paintTraverseDetour) {
    g_paintTraverseDetour->unHook();
    g_paintTraverseDetour.reset();
  }
  g_originalPaintTraverseDetour = 0;

  if (!g_panelVtable || !g_originalPaintTraverse) {
    return;
  }

  if (g_activePaintTraverseIndex < 0) {
    return;
  }

  DWORD oldProtect = 0;
  if (VirtualProtect(&g_panelVtable[g_activePaintTraverseIndex], sizeof(void *),
                     PAGE_EXECUTE_READWRITE, &oldProtect)) {
    g_panelVtable[g_activePaintTraverseIndex] =
        reinterpret_cast<void *>(g_originalPaintTraverse);
    DWORD unused = 0;
    VirtualProtect(&g_panelVtable[g_activePaintTraverseIndex], sizeof(void *),
                   oldProtect, &unused);
  }

  g_originalPaintTraverse = nullptr;
  g_panelVtable = nullptr;
  g_installed = false;
  g_activePaintTraverseIndex = -1;
  g_loggedPaintTraverseEntry = false;
}

bool PaintTraverseHook::IsInstalled() { return g_installed; }
