#!/bin/bash

# NSCppPluginTemplate Build Script (Optimized)
# Builds the plugin as a Windows DLL and copies it to Northstar

set -e  # Exit on any error

# ==================== Configuration ====================
PROJECT_NAME="nh"
BUILD_DIR="build"
NORTHSTAR_PLUGINS_DIR="$HOME/.local/share/Steam/steamapps/common/Titanfall2/R2Northstar/plugins"
DLL_SOURCE="bin/lib${PROJECT_NAME}.dll"
OUTPUT_DLL="lib${PROJECT_NAME}.dll"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}=== NSCppPluginTemplate Build Script ===${NC}"

# Check dependencies
for cmd in x86_64-w64-mingw32-gcc cmake; do
    if ! command -v "$cmd" &> /dev/null; then
        echo -e "${RED}Error: $cmd not found!${NC}"
        echo "Install with: sudo pacman -S mingw-w64-gcc cmake"
        exit 1
    fi
done

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# ==================== Toolchain ====================
if [ ! -f "mingw-toolchain.cmake" ]; then
    echo -e "${YELLOW}Creating MinGW toolchain file...${NC}"
    cat > mingw-toolchain.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF
fi

# ==================== CMake Configure (only when needed) ====================
CMAKE_ARGS=(
    -DCMAKE_TOOLCHAIN_FILE=mingw-toolchain.cmake
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
)

if [ ! -f "CMakeCache.txt" ] || [ ../CMakeLists.txt -nt CMakeCache.txt ]; then
    echo -e "${YELLOW}Configuring with CMake...${NC}"
    cmake .. "${CMAKE_ARGS[@]}"
else
    echo -e "${YELLOW}CMake cache up to date, skipping configure...${NC}"
fi

# ==================== Build ====================
echo -e "${YELLOW}Building project...${NC}"
cmake --build . --config Release --parallel

# Check output
if [ ! -f "$DLL_SOURCE" ]; then
    echo -e "${RED}Error: Build failed! $DLL_SOURCE not found.${NC}"
    exit 1
fi

echo -e "${GREEN}Build successful! Created: $DLL_SOURCE${NC}"

# ==================== Copy to Northstar ====================
if [ ! -d "$NORTHSTAR_PLUGINS_DIR" ]; then
    echo -e "${YELLOW}Creating Northstar plugins directory...${NC}"
    mkdir -p "$NORTHSTAR_PLUGINS_DIR"
fi

echo -e "${YELLOW}Copying DLL to Northstar plugins...${NC}"
cp -f "$DLL_SOURCE" "$NORTHSTAR_PLUGINS_DIR/"

if [ -f "$NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL" ]; then
    echo -e "${GREEN}Success! Plugin installed to:${NC}"
    echo "  $NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL"
    
    echo -e "${BLUE}File details:${NC}"
    ls -lh "$NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL"
    file "$NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL"
else
    echo -e "${RED}Error: Failed to copy DLL!${NC}"
    exit 1
fi

echo -e "${GREEN}=== Build and installation complete! ===${NC}"
