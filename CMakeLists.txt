cmake_minimum_required(VERSION 3.16)

project("CppPluginExample" VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent for downloading dependencies
include(FetchContent)

# Fetch fmt library
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        10.2.1
)
FetchContent_MakeAvailable(fmt)

# Fetch PolyHook library
FetchContent_Declare(
    polyhook
    GIT_REPOSITORY https://github.com/stevemk14ebr/PolyHook_2_0.git
    GIT_TAG        master  # or use a specific tag like "2.0.0"
)
FetchContent_MakeAvailable(polyhook)

# Fetch ImGui library
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.90.4
)
FetchContent_MakeAvailable(imgui)

# Create ImGui library target
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# Core source files
set(CORE_SOURCES
    "src/pch.h"
    "src/pch.cpp"
    "src/CppPluginExample.h"
    "src/CppPluginExample.cpp"
    "src/PluginConfiguration.h"
    "src/Include.h"
    "src/Config.h"
    "src/Config.cpp"
)

# Interface sources
set(INTERFACE_SOURCES
    "src/interfaces/interfaces.h"
    "src/interfaces/interfaces.cpp"
    "src/interfaces/Northstar.h"
    "src/interfaces/Northstar.cpp"
    "src/interfaces/Northstar/INSSys.h"
)

# Exposed interface sources
set(EXPOSED_INTERFACE_SOURCES
    "src/interfaces/exposed/IPluginId/IPluginId.h"
    "src/interfaces/exposed/IPluginId/IPluginId.cpp"
    "src/interfaces/exposed/IPluginCallbacks/IPluginCallbacks.h"
    "src/interfaces/exposed/IPluginCallbacks/IPluginCallbacks.cpp"
)

# R2SDK class sources
set(R2SDK_CLASS_SOURCES
    "src/r2sdk/classes/CBaseEntity.h"
    "src/r2sdk/classes/CBaseEntity.cpp"
    "src/r2sdk/classes/CGlobalVarsBase.h"
    "src/r2sdk/classes/CInput.h"
    "src/r2sdk/classes/CInputSystem.h"
    "src/r2sdk/classes/ClientClass.h"
    "src/r2sdk/classes/ClientClass.cpp"
    "src/r2sdk/classes/RecvTable.h"
    "src/r2sdk/classes/RecvProp.h"
    "src/r2sdk/classes/ConVar.h"
    "src/r2sdk/classes/IConVar.h"
    "src/r2sdk/classes/ConCommandBase.h"
)

# R2SDK interface sources
set(R2SDK_INTERFACE_SOURCES
    "src/r2sdk/interfaces/IBaseClientDLL.h"
    "src/r2sdk/interfaces/IBaseClientDLL.cpp"
    "src/r2sdk/interfaces/IClientRenderable.h"
    "src/r2sdk/interfaces/IClientRenderable.cpp"
    "src/r2sdk/interfaces/IClientThinkable.h"
    "src/r2sdk/interfaces/IClientUnknown.h"
    "src/r2sdk/interfaces/IClientNetworkable.h"
    "src/r2sdk/interfaces/IClientNetworkable.cpp"
    "src/r2sdk/interfaces/IClientEntity.h"
    "src/r2sdk/interfaces/IClientEntityList.h"
    "src/r2sdk/interfaces/IClientEntityList.cpp"
    "src/r2sdk/interfaces/IVEngineClient.h"
    "src/r2sdk/interfaces/IVEngineClient.cpp"
    "src/r2sdk/interfaces/ICvar.h"
    "src/r2sdk/interfaces/ICvar.cpp"
    "src/r2sdk/interfaces/IAppSystem.h"
    "src/r2sdk/interfaces/IHandleEntity.h"
    "src/r2sdk/interfaces/ITraceFilter.h"
    "src/r2sdk/interfaces/IEngineTrace.h"
    "src/r2sdk/interfaces/IEngineTrace.cpp"
    "src/r2sdk/interfaces/IViewRender.h"
    "src/r2sdk/interfaces/IViewRender.cpp"
    "src/r2sdk/interfaces/CMatSystemSurface.h"
    "src/r2sdk/interfaces/IPanel.h"
)

# R2SDK main header
set(R2SDK_CORE
    "src/r2sdk/r2sdk.h"
)

# Utility sources
set(UTILITY_SOURCES
    "src/utilities/Memory.h"
    "src/utilities/Memory.cpp"
    "src/utilities/Math.h"
    "src/utilities/Math.cpp"
    "src/utilities/PatternScanner.h"
    "src/utilities/PatternScanner.cpp"
    "src/utilities/Types.h"
    "src/utilities/DllHooker.h"
    "src/utilities/DllHooker.cpp"
    "src/hooks/PaintTraverseHook.h"
    "src/hooks/PaintTraverseHook.cpp"
    "src/hooks/CreateMoveHook.h"
    "src/hooks/CreateMoveHook.cpp"
)

# Feature sources (only include files that exist)
set(FEATURE_SOURCES
    "src/features/features.h"
)

# GUI sources
set(GUI_SOURCES
    "src/gui/GuiManager.h"
    "src/gui/GuiManager.cpp"
)

# Input sources
set(INPUT_SOURCES
    "src/input/InputManager.h"
    "src/input/InputManager.cpp"
)

# Core system sources
set(CORE_SOURCES_NEW
    "src/core/IFeatureModule.h"
    "src/core/EventSystem.h"
    "src/core/EventSystem.cpp"
    "src/core/PluginCallbacksManager.h"
    "src/core/PluginCallbacksManager.cpp"
)

# Feature module sources
set(MODULE_SOURCES
    "src/modules/LobbyLoggingModule.h"
    "src/modules/LobbyLoggingModule.cpp"
    "src/modules/DebugModule.h"
    "src/modules/DebugModule.cpp"
    "src/modules/NetVarModule.h"
    "src/modules/NetVarModule.cpp"
    "src/modules/ESP.h"
    "src/modules/ESP.cpp"
    "src/modules/AirstuckModule.h"
    "src/modules/AirstuckModule.cpp"
    "src/modules/Speed.h"
    "src/modules/Speed.cpp"
    "src/modules/Aimbot.h"
    "src/modules/Aimbot.cpp"
)

# NetVar and initialization sources
set(NETVAR_SOURCES
    "src/NetVarManager.h"
    "src/NetVarManager.cpp"
    "src/Initialize.h"
    "src/Initialize.cpp"
)

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${INTERFACE_SOURCES}
    ${EXPOSED_INTERFACE_SOURCES}
    ${R2SDK_CLASS_SOURCES}
    ${R2SDK_INTERFACE_SOURCES}
    ${R2SDK_CORE}
    ${UTILITY_SOURCES}
    ${FEATURE_SOURCES}
    ${GUI_SOURCES}
    ${INPUT_SOURCES}
    ${CORE_SOURCES_NEW}
    ${MODULE_SOURCES}
    ${NETVAR_SOURCES}
)

# Create the shared library
add_library(CppPluginExample SHARED ${ALL_SOURCES})

# Set target properties
set_target_properties(CppPluginExample PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Enable precompiled headers
target_precompile_headers(CppPluginExample PRIVATE "src/pch.h")

# Include directories
target_include_directories(CppPluginExample PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/r2sdk"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/interfaces"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utilities"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/features"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gui"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/input"
)

# Link libraries
target_link_libraries(CppPluginExample PRIVATE 
    fmt::fmt
    PolyHook_2
    imgui
)

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(CppPluginExample PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    
    # Link DirectX 11 libraries for GUI
    target_link_libraries(CppPluginExample PRIVATE 
        d3d11
        dxgi
        d3dcompiler  # For D3DCompile function
        dwmapi       # For DWM functions
        -static-libgcc 
        -static-libstdc++
    )
    
    set_target_properties(CppPluginExample PROPERTIES
        LINK_FLAGS "-static -static-libgcc -static-libstdc++"
    )
    
    # Set output directory for Windows builds
    set_target_properties(CppPluginExample PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
endif()

# Compiler-specific settings
if(MSVC)
    target_compile_options(CppPluginExample PRIVATE /W4)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(CppPluginExample PRIVATE 
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )
endif()

# Custom target to copy plugin to Northstar directory (if specified)
if(DEFINED NORTHSTAR_PLUGINS_DIR)
    add_custom_command(TARGET CppPluginExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        $<TARGET_FILE:CppPluginExample> 
        "${NORTHSTAR_PLUGINS_DIR}/"
        COMMENT "Copying plugin to Northstar plugins directory: ${NORTHSTAR_PLUGINS_DIR}"
    )
endif()

# Print build information
message(STATUS "Building CppPluginExample v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
if(DEFINED NORTHSTAR_PLUGINS_DIR)
    message(STATUS "Northstar plugins directory: ${NORTHSTAR_PLUGINS_DIR}")
endif()
# Seriously, add an assert size and offsets of every structure you use from the game. It's a lifesaver.
