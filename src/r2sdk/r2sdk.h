#pragma once

#include "../utilities/Types.h"

// Forward declarations
class IConVar;
class ConCommand;
class IConsoleDisplayFunc;

// Base classes first
#include "ButtonCode.h"
#include "classes/CGlobalVarsBase.h"
#include "classes/CInput.h"
#include "classes/CInputSystem.h"
#include "classes/CTraceFilter.h"
#include "classes/CUserCmd.h"
#include "classes/ConCommandBase.h"
#include "classes/ConVar.h"
#include "classes/IConVar.h"
#include "classes/IConsoleDisplayFunc.h"
#include "classes/RecvProp.h"
#include "classes/RecvTable.h"

// Entity classes
#include "classes/CBaseEntity.h"
#include "classes/ClientClass.h"

// Interfaces
#include "interfaces/CMatSystemSurface.h"
#include "interfaces/IAppSystem.h"
#include "interfaces/IBaseClientDLL.h"
#include "interfaces/IClientEntity.h"
#include "interfaces/IClientEntityList.h"
#include "interfaces/ICvar.h"
#include "interfaces/IEngineTrace.h"
#include "interfaces/IHandleEntity.h"
#include "interfaces/IPanel.h"
#include "interfaces/ITraceFilter.h"
#include "interfaces/IVEngineClient.h"

struct Interfaces {
  IBaseClientDLL *Client;
  IClientEntityList *ClientEntityList;
  IVEngineClient *EngineClient;
  ICvar *Cvar;
  IEngineTrace *EngineTrace;
  IPanel *Panel;
};

inline Interfaces g_Interfaces;

namespace r2sdk::IBaseClientDLL::Index {
constexpr int GetAllClasses = 11;
}

namespace r2sdk::IClientEntityList::Index {
constexpr int GetClientNetworkable = 0;
constexpr int GetClientEntity = 3;
constexpr int NumberOfEntities = 5;
constexpr int GetHighestEntity = 6;
constexpr int SetMaxEntities = 7;
constexpr int GetMaxEntities = 8;
} // namespace r2sdk::IClientEntityList::Index

namespace r2sdk::IVEngineClient::Index {
constexpr int GetLocalPlayer = 43;
constexpr int GetPlayerInfo = 28;
constexpr int IsInGame = 200;
constexpr int ClientSendMessage = 277;
} // namespace r2sdk::IVEngineClient::Index

namespace r2sdk::IEngineTrace::Index {
constexpr int TraceRay = 3;
} // namespace r2sdk::IEngineTrace::Index

namespace r2sdk::IViewRender::Index {
constexpr int GetWorldToScreenMatrix = 14;
constexpr int HasWorldToScreenMatrix = 15;
} // namespace r2sdk::IViewRender::Index

namespace r2sdk::IClientRenderable::Index {
constexpr int SetupBones = 13;
} // namespace r2sdk::IClientRenderable::Index

namespace r2sdk::IClientNetworkable::Index {
constexpr int GetClientClass = 2;
} // namespace r2sdk::IClientNetworkable::Index

inline uintptr_t BaseAddress;
inline HMODULE ClientModule;
inline uintptr_t GameMovementPtr;
inline uintptr_t LocalPlayerPtr;
inline uintptr_t ViewRenderPtr;