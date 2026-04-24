#pragma once
#include <stdint.h>

#define PLUGIN_CONTEXT_DEDICATED 0x1
#define PLUGIN_CONTEXT_CLIENT 0x2

struct Color
{
	int r = 0;
	int g = 0;
	int b = 0;
	int a = 255;

	Color() = default;
	Color(int r, int g, int b) : r(r), g(g), b(b), a(255) {}
	Color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {}

	inline int64_t bitflag() const
	{
		return (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16);
	}
};

// this is the plugin name
#define PLUGIN_NAME "CppExamplePlugin"

// this is the name northstar uses when this plugin is logging to the console
#define PLUGIN_LOG_NAME "CppPlugin"

// this is the color northstar will render the log name in
#define PLUGIN_COLOR Color(165, 137, 193 );

// this is the name of the squirrel constant northstar creates
#define PLUGIN_DEPENDENCY_NAME "CPP_EXAMPLE_PLUGIN"

// this is a bitfield that determines if Northstar should load this plugin
#define PLUGIN_CONTEXT PLUGIN_CONTEXT_CLIENT;
