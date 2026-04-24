#include "pch.h"

#include "IClientNetworkable.h"

ClientClass* IClientNetworkable::GetClientClass()
{
	return Memory::InvokeVTableFunction<ClientClass*>(this, r2sdk::IClientNetworkable::Index::GetClientClass);
}