#include "../../pch.h"

// IClientNetworkable* IClientEntityList::GetClientNetworkable(int entnum)
// {
// 	return MadFramework::Memory::InvokeVTableFunction<IClientNetworkable*>(this, SourceEngine::IClientEntityList::Index::GetClientNetworkable, entnum);
// }

IClientEntity* IClientEntityList::GetClientEntity(int entnum)
{
	return Memory::InvokeVTableFunction<IClientEntity*>(this, r2sdk::IClientEntityList::Index::GetClientEntity, entnum);
}

int IClientEntityList::NumberOfEntities(bool bIncludeNonNetworkable)
{
	return Memory::InvokeVTableFunction<int>(this, r2sdk::IClientEntityList::Index::NumberOfEntities, bIncludeNonNetworkable);
}

int IClientEntityList::GetHighestEntityIndex()
{
	return Memory::InvokeVTableFunction<int>(this, r2sdk::IClientEntityList::Index::GetHighestEntity);
}

void IClientEntityList::SetMaxEntities(int maxents)
{
	return Memory::InvokeVTableFunction<void>(this, r2sdk::IClientEntityList::Index::SetMaxEntities, maxents);
}

int IClientEntityList::GetMaxEntities()
{
	return Memory::InvokeVTableFunction<int>(this, r2sdk::IClientEntityList::Index::GetMaxEntities);
}