#include "CppPluginExample.h"
#include "pch.h"

void InitCppExamplePlugin(HMODULE pluginHandle, HMODULE nsHandle) {
  g_handle = pluginHandle;

  if (!InitNSSys(nsHandle)) {
    // could not get the NSSys interface
    // (something went very wrong or the installed Northstar version is very
    // old)
    printf("could not initialize NSSys");
    return;
  }

  g_nssys->Log(pluginHandle, LogLevel::INFO, "Hello World");
}
