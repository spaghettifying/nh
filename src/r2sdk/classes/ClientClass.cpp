#include "../../pch.h"
#include "ClientClass.h"

const char* ClientClass::GetName()
{
	// TODO: Implement with proper offsets
	return m_pNetworkName;
}

int ClientClass::GetClassID()
{
	// TODO: Implement with proper offsets  
	return m_ClassID;
}