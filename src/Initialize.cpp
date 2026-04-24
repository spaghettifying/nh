#include "Initialize.h"
#include "r2sdk/classes/CGlobalVarsBase.h"
#include "r2sdk/classes/CInput.h"
#include "r2sdk/classes/CInputSystem.h"
#include "r2sdk/interfaces/CMatSystemSurface.h"
#include "utilities/PatternScanner.h"

namespace {
void LogInitMessage(LogLevel level, const char* message) {
  if (g_nssys && g_handle) {
    g_nssys->Log(g_handle, level, message);
  }
}

void* CreateInterfaceWithVersionRange(const char* moduleName,
                                      const char* prefix,
                                      int minVersion,
                                      int maxVersion,
                                      int step = -1) {
  if (!moduleName || !prefix || minVersion < 0 || maxVersion < 0 ||
      minVersion > maxVersion) {
    return nullptr;
  }

  if (step == 0) {
    step = -1;
  }

  int start = (step < 0) ? maxVersion : minVersion;
  int end = (step < 0) ? minVersion : maxVersion;

  for (int version = start; (step < 0) ? (version >= end) : (version <= end);
       version += step) {
    const std::string name = fmt::format("{}{:03}", prefix, version);
    void* iface = Initialize::CreateInterface(moduleName, name.c_str());
    if (iface) {
      LogInitMessage(
          LogLevel::INFO,
          fmt::format("Resolved {} interface: {}", moduleName, name).c_str());
      return iface;
    }
  }

  return nullptr;
}

bool ResolvePointerFromSignature(void** outPointer, const char* moduleName,
                                 const char* signature,
                                 const char* pointerName,
                                 int instructionSize = 7,
                                 int displacementOffset = 3) {
  if (!outPointer || !moduleName || !signature || !pointerName) {
    return false;
  }

  if (*outPointer) {
    return true;
  }

  HMODULE module = GetModuleHandleA(moduleName);
  if (!module) {
    LogInitMessage(
        LogLevel::WARN,
        fmt::format("{} not loaded while resolving {}", moduleName, pointerName)
            .c_str());
    return false;
  }

  const uintptr_t sigAddress = PatternScanner::FindPattern(module, signature);
  if (!sigAddress) {
    LogInitMessage(
        LogLevel::WARN,
        fmt::format("Failed to find signature for {}", pointerName).c_str());
    return false;
  }

  const uintptr_t resolvedAddress = PatternScanner::ResolveRipRelative(
      sigAddress, instructionSize, displacementOffset);
  if (!resolvedAddress) {
    LogInitMessage(LogLevel::WARN,
                   fmt::format("Failed to resolve RIP-relative address for {}",
                               pointerName)
                       .c_str());
    return false;
  }

  *outPointer = *reinterpret_cast<void**>(resolvedAddress);
  if (*outPointer) {
    LogInitMessage(LogLevel::INFO,
                   fmt::format("Resolved {} via signature", pointerName).c_str());
    return true;
  }

  LogInitMessage(LogLevel::WARN,
                 fmt::format("Resolved {} pointer was null", pointerName)
                     .c_str());
  return false;
}
} // namespace

using namespace Initialize;
// typedef void *(*CreateInterfaceFn)(const char *Name, int *ReturnCode);
void *Initialize::CreateInterface(const char *ModuleName,
                                  const char *InterfaceName) {

  g_nssys->Log(g_handle, LogLevel::INFO, "Creating interface");

  HMODULE hModule = GetModuleHandleA(ModuleName);
  if (!hModule) {
    g_nssys->Log(
        g_handle, LogLevel::ERR,
        fmt::format("Failed to get module handle for {}", ModuleName).c_str());
    return nullptr;
  }
  g_nssys->Log(g_handle, LogLevel::INFO, "Got hModule");

  FARPROC Address = GetProcAddress(hModule, "CreateInterface");
  if (!Address) {
    g_nssys->Log(g_handle, LogLevel::ERR,
                 "Failed to get CreateInterface function");
    return nullptr;
  }
  g_nssys->Log(g_handle, LogLevel::INFO, "GotProcAddress");

  const auto CreateInterface = reinterpret_cast<CreateInterfaceFn>(Address);
  if (!CreateInterface) {
    g_nssys->Log(g_handle, LogLevel::ERR,
                 "CreateInterface function pointer is null");
    return nullptr;
  }
  g_nssys->Log(g_handle, LogLevel::INFO, "Reinterpreting Cast");

  if (!InterfaceName) {
    g_nssys->Log(g_handle, LogLevel::ERR, "InterfaceName is null");
    return nullptr;
  }

  g_nssys->Log(g_handle, LogLevel::INFO,
               fmt::format("{} : {} ", ModuleName, InterfaceName).c_str());

  auto Ret = CreateInterface(InterfaceName, nullptr);

  // printf("Module: %s : %s : %p \n", ModuleName, InterfaceName, Ret);

  return Ret;
}

void Initialize::InitPointers() {
  g_Interfaces.Client =
      (IBaseClientDLL *)Initialize::CreateInterface("client.dll", "VClient018");
  if (!g_Interfaces.Client)
    g_nssys->Log(g_handle, LogLevel::ERR, "Failed to create IBaseClientDLL");

  g_Interfaces.ClientEntityList =
      (IClientEntityList *)Initialize::CreateInterface("client.dll",
                                                       "VClientEntityList003");
  if (!g_Interfaces.ClientEntityList)
    g_nssys->Log(g_handle, LogLevel::ERR, "Failed to create IClientEntityList");

  g_Interfaces.EngineClient = (IVEngineClient *)Initialize::CreateInterface(
      "engine.dll", "VEngineClient013");
  if (!g_Interfaces.EngineClient)
    g_nssys->Log(g_handle, LogLevel::ERR, "Failed to create IVEngineClient");

  g_Interfaces.EngineTrace = (IEngineTrace *)Initialize::CreateInterface(
      "engine.dll", "EngineTraceClient004");
  if (!g_Interfaces.EngineTrace)
    g_nssys->Log(g_handle, LogLevel::WARN,
                 "Failed to create IEngineTrace (EngineTraceClient004)");

  InputSystem = (CInputSystem *)Initialize::CreateInterface(
      "inputsystem.dll", "InputSystemVersion001");
  if (!InputSystem)
    g_nssys->Log(g_handle, LogLevel::ERR,
                 "Failed to create CInputSystem (InputSystemVersion001)");

  // Some setups expose input through client-side interface names.
  Input = (CInput *)Initialize::CreateInterface("client.dll", "VInput001");
  if (!Input) {
    Input = (CInput *)Initialize::CreateInterface("client.dll", "Input001");
  }
  if (!Input)
    g_nssys->Log(g_handle, LogLevel::WARN,
                 "CInput interface not found (VInput001/Input001)");

  if (!MatSystemSurface) {
    MatSystemSurface = reinterpret_cast<CMatSystemSurface *>(
        CreateInterfaceWithVersionRange("vguimatsurface.dll", "VGUI_Surface",
                                        1, 64));
  }
  if (!MatSystemSurface)
    g_nssys->Log(g_handle, LogLevel::WARN,
                 "CMatSystemSurface interface not found (VGUI_Surface###)");

  if (!g_Interfaces.Panel) {
    g_Interfaces.Panel = reinterpret_cast<IPanel *>(
        CreateInterfaceWithVersionRange("vgui2.dll", "VGUI_Panel", 1, 64));
  }
  VGuiPanel = g_Interfaces.Panel;
  if (!g_Interfaces.Panel)
    g_nssys->Log(g_handle, LogLevel::WARN,
                 "IPanel interface not found (VGUI_Panel###)");

  ResolvePointerFromSignature(
      reinterpret_cast<void**>(&g_pGlobalVars), "client.dll",
      "48 8B 05 ? ? ? ? 48 8B D9 F3 0F 10 48 ? E8 ? ? ? ? 48 8B",
      "g_pGlobalVars");

  ClientModule = GetModuleHandle("client.dll");
  const uintptr_t gameMovementInstr = PatternScanner::FindPattern(
      ClientModule, "48 89 05 ? ? ? ? 48 85 C9 74 10 48 8B 01 48 8B");
  if (gameMovementInstr) {
    GameMovementPtr =
        PatternScanner::ResolveRipRelative(gameMovementInstr, 7, 3);
    if (GameMovementPtr) {
      LocalPlayerPtr = *reinterpret_cast<uintptr_t *>(GameMovementPtr) + 0x8;
    }
  }

  const uintptr_t viewRenderInstr = PatternScanner::FindPattern(
      ClientModule,
      "48 8B 0D ? ? ? ? 83 CA FF 48 8B 01 FF 50 60 48 8D 15 ? ? ? ? 48 8D 88");
  if (viewRenderInstr) {
    ViewRenderPtr = PatternScanner::ResolveRipRelative(viewRenderInstr, 7, 3);
  }
}