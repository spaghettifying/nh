#pragma once
#include <windows.h>

class RecvTable;

// typedef class void* (*CreateEventFn)();
// typedef class void* (*CreateClientClassFn)(int entnum, int serialNum);

class ClientClass
{
public:
	void*		            m_pCreateFn;
	void*       			m_pCreateEventFn;	
	char*					m_pNetworkName;
	RecvTable*				m_pRecvTable;
	ClientClass*			m_pNext;
	int						m_ClassID;

public:
	const char* GetName();
	int GetClassID();
};