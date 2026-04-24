#pragma once

#include "./Northstar/INSSys.h"


// This errors for some reason idk
// using CreateInterface = void* (*)(const char*, int*);
// CreateInterface g_ns_CreateInterface;
typedef void* (*CreateInterface_T)(const char*, int*);
static CreateInterface_T g_ns_CreateInterface;

inline INSSys* g_nssys;

bool InitNSSys(HMODULE nsHandle);
