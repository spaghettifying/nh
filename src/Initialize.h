#pragma once

#include "pch.h"

namespace Initialize {
typedef void *(*CreateInterfaceFn)(const char *Name, int *ReturnCode);
void* CreateInterface(const char *ModuleName, const char *InterfaceName);
void InitPointers();
} // namespace Initialize
