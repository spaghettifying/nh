#include "../../pch.h"

int IVEngineClient::GetLocalPlayer()
{
	return Memory::InvokeVTableFunction<int>(this, r2sdk::IVEngineClient::Index::GetLocalPlayer);
}

bool IVEngineClient::GetPlayerInfo(int ent_num, player_info_t* pinfo)
{
	return Memory::InvokeVTableFunction<bool>(this, r2sdk::IVEngineClient::Index::GetPlayerInfo, ent_num, pinfo);
}

bool IVEngineClient::IsInGame(void)
{
	return Memory::InvokeVTableFunction<bool>(this, r2sdk::IVEngineClient::Index::IsInGame);
}

void IVEngineClient::SendClientMessage(const char* message) noexcept
{
	//48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC 50 01 00 00 engine.dll
	return Memory::InvokeVTableFunction<void>(this, r2sdk::IVEngineClient::Index::ClientSendMessage, message, 1, false);
}