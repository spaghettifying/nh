#pragma once

#include "pch.h"

// Forward declarations
class RecvTable;

#define NETVAR(_name, type, table, name) inline type &_name() \
{ \
	static const int nOffset = NetVarManager::GetNetVar(table, name); \
	return *reinterpret_cast<type *>(reinterpret_cast<std::uintptr_t>(this) + nOffset); \
}

#define NETVAR_OFFSET(_name, type, offset) inline type &_name() \
{ \
	return *reinterpret_cast<type *>(reinterpret_cast<std::uintptr_t>(this) + offset); \
}

#define NETVAR_STRING(_name, type, table, name) inline type *_name() \
{ \
	static const int nOffset = NetVarManager::GetNetVar(table, name); \
	return reinterpret_cast<type *>(reinterpret_cast<std::uintptr_t>(this) + nOffset); \
}

#define NETVAR_OFFSET_STRING(_name, type, offset) inline type _name() \
{ \
	return reinterpret_cast<type>(reinterpret_cast<std::uintptr_t>(this) + offset); \
}

namespace NetVarManager {
int NetVarOffset(RecvTable *pRecvTable, const char *NetVarName);

int GetNetVar(const char *TableName, const char *NetVarName);

void Dump();
}