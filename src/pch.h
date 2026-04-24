#pragma once

// Windows headers
#include <windows.h>
#include <intrin.h>

// Standard library headers
#include <cstdint>
#include <span>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <cwctype>
#include <numbers>
#include <filesystem>
#include <array>
#include <unordered_set>
#include <random>
#include <expected>
#include <unordered_map>
#include <mutex>
#include <regex>
#include <fmt/core.h>
#include <math.h>

// Project configuration
#include "PluginConfiguration.h"

// Core project interfaces
#include "interfaces/interfaces.h"
#include "interfaces/Northstar.h"

// r2sdk
#include "r2sdk/r2sdk.h"
#include "r2sdk/vars.h"

// features
#include "features/features.h"

inline HMODULE g_handle;

#include "NetVarManager.h"
#include "Initialize.h"
#include "Config.h"

// utilities
#include "utilities/Memory.h"
#include "utilities/Types.h"
#include "utilities/PatternScanner.h"
#include "utilities/Math.h"

#include "input/InputManager.h"