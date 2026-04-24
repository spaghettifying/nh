#include "CBaseEntity.h"
#include "pch.h"

bool CBaseEntity::IsTitan() noexcept
{
	return Memory::InvokeVTableFunction<bool>(this, 0x96);
}

bool CBaseEntity::IsPlayer() noexcept
{
  g_nssys->Log(g_handle, LogLevel::INFO, "isplayer called");
  return Memory::InvokeVTableFunction<bool>(this, 0x95);
}

bool CBaseEntity::IsPlayerDecoy() noexcept
{
	return Memory::InvokeVTableFunction<bool>(this, 0x9D);
}

bool CBaseEntity::IsNpc() noexcept
{
	return Memory::InvokeVTableFunction<bool>(this, 0x9C);
}