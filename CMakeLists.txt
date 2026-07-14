cmake_minimum_required(VERSION 3.20)

project("nh" VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        10.2.1
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    polyhook
    GIT_REPOSITORY https://github.com/stevemk14ebr/PolyHook_2_0.git
    GIT_TAG        master  # or use a specific tag like "2.0.0"
)
FetchContent_MakeAvailable(polyhook)

set(CORE_SOURCES
    "src/nh.hh"
    "src/nh.cc"
    "src/PluginConfiguration.hh"
)

set(INTERFACE_SOURCES
    "src/interfaces/interfaces.hh"
    "src/interfaces/interfaces.cc"
    "src/interfaces/Northstar.hh"
    "src/interfaces/Northstar.cc"
    "src/interfaces/Northstar/INSSys.hh"
)

set(EXPOSED_INTERFACE_SOURCES
    "src/interfaces/exposed/IPluginId/IPluginId.hh"
    "src/interfaces/exposed/IPluginId/IPluginId.cc"
    "src/interfaces/exposed/IPluginCallbacks/IPluginCallbacks.hh"
    "src/interfaces/exposed/IPluginCallbacks/IPluginCallbacks.cc"
)

# set(R2SDK_CLASS_SOURCES
#     "src/r2sdk/classes/CBaseEntity.h"
#     "src/r2sdk/classes/CBaseEntity.cpp"
#     "src/r2sdk/classes/CGlobalVarsBase.h"
#     "src/r2sdk/classes/CInput.h"
#     "src/r2sdk/classes/CInputSystem.h"
#     "src/r2sdk/classes/ClientClass.h"
#     "src/r2sdk/classes/ClientClass.cpp"
#     "src/r2sdk/classes/RecvTable.h"
#     "src/r2sdk/classes/RecvProp.h"
#     "src/r2sdk/classes/ConVar.h"
#     "src/r2sdk/classes/IConVar.h"
#     "src/r2sdk/classes/ConCommandBase.h"
# )

# set(R2SDK_INTERFACE_SOURCES
#     "src/r2sdk/interfaces/IBaseClientDLL.h"
#     "src/r2sdk/interfaces/IBaseClientDLL.cpp"
#     "src/r2sdk/interfaces/IClientRenderable.h"
#     "src/r2sdk/interfaces/IClientRenderable.cpp"
#     "src/r2sdk/interfaces/IClientThinkable.h"
#     "src/r2sdk/interfaces/IClientUnknown.h"
#     "src/r2sdk/interfaces/IClientNetworkable.h"
#     "src/r2sdk/interfaces/IClientNetworkable.cpp"
#     "src/r2sdk/interfaces/IClientEntity.h"
#     "src/r2sdk/interfaces/IClientEntityList.h"
#     "src/r2sdk/interfaces/IClientEntityList.cpp"
#     "src/r2sdk/interfaces/IVEngineClient.h"
#     "src/r2sdk/interfaces/IVEngineClient.cpp"
#     "src/r2sdk/interfaces/ICvar.h"
#     "src/r2sdk/interfaces/ICvar.cpp"
#     "src/r2sdk/interfaces/IAppSystem.h"
#     "src/r2sdk/interfaces/IHandleEntity.h"
#     "src/r2sdk/interfaces/ITraceFilter.h"
#     "src/r2sdk/interfaces/IEngineTrace.h"
#     "src/r2sdk/interfaces/IEngineTrace.cpp"
#     "src/r2sdk/interfaces/IViewRender.h"
#     "src/r2sdk/interfaces/IViewRender.cpp"
#     "src/r2sdk/interfaces/CMatSystemSurface.h"
#     "src/r2sdk/interfaces/IPanel.h"
# )

# set(R2SDK_CORE
#     "src/r2sdk/r2sdk.h"
# )

set(ALL_SOURCES
    ${CORE_SOURCES}
    ${INTERFACE_SOURCES}
    ${EXPOSED_INTERFACE_SOURCES}
)

add_library(nh SHARED ${ALL_SOURCES})

set_target_properties(nh PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

target_include_directories(nh PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(nh PRIVATE 
    fmt::fmt
    PolyHook_2
)

if(WIN32)
    target_compile_definitions(nh PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    
    target_link_libraries(nh PRIVATE 
        d3d11
        dxgi
        d3dcompiler  # For D3DCompile function
        dwmapi       # For DWM functions
        -static-libgcc 
        -static-libstdc++
    )
    
    set_target_properties(nh PROPERTIES
        LINK_FLAGS "-static -static-libgcc -static-libstdc++"
    )
    
    set_target_properties(nh PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
endif()

if(MSVC)
    target_compile_options(nh PRIVATE /W4)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(nh PRIVATE 
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )
endif()

if(DEFINED NORTHSTAR_PLUGINS_DIR)
    add_custom_command(TARGET nh POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        $<TARGET_FILE:nh> 
        "${NORTHSTAR_PLUGINS_DIR}/"
        COMMENT "Copying plugin to Northstar plugins directory: ${NORTHSTAR_PLUGINS_DIR}"
    )
endif()

message(STATUS "Building nh v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
if(DEFINED NORTHSTAR_PLUGINS_DIR)
    message(STATUS "Northstar plugins directory: ${NORTHSTAR_PLUGINS_DIR}")
endif()
