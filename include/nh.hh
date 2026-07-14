#pragma once

// Plugin settings are located in PluginConfiguration.h

#include "interfaces/interfaces.hh"
#include <windows.h>

// The real "entry" is the IPluginCallbacks::Init callback.
// You can find it at src/interfaces/exposed/IPluginCallbacks.cpp

// src/interfaces/exposed are interfaces this plugin exposes
// src/interfaces/Northstar are interfaces that NS exposes and which are used by
// this plugin. src/interfaces/Northstar.cpp manages interfaces acquired from
// Northstar

// handle of this plugin
inline HMODULE g_handle;

// This gets called by IPluginCallbacks::Init
void InitCppExamplePlugin(HMODULE pluginHandle, HMODULE nsHandle);
