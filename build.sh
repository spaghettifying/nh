#!/bin/bash

# NSCppPluginTemplate Build Script
# Builds the plugin as a Windows DLL and copies it to Northstar plugins directory

set -e  # Exit on any error

# Configuration
PROJECT_NAME="CppPluginExample"
BUILD_DIR="build-windows"
NORTHSTAR_PLUGINS_DIR="$HOME/Games/Steam/steamapps/common/Titanfall2/R2Northstar/plugins"
OUTPUT_DLL="lib${PROJECT_NAME}.dll"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== NSCppPluginTemplate Build Script ===${NC}"

# Check if MinGW-w64 is installed
if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    echo -e "${RED}Error: MinGW-w64 cross-compiler not found!${NC}"
    echo "Please install it with: sudo apt install mingw-w64"
    exit 1
fi

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake not found!${NC}"
    echo "Please install it with: sudo apt install cmake"
    exit 1
fi

# Create build directory
echo -e "${YELLOW}Creating build directory...${NC}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Create toolchain file if it doesn't exist
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

# Configure with CMake
echo -e "${YELLOW}Configuring with CMake...${NC}"
cmake .. -DCMAKE_TOOLCHAIN_FILE=mingw-toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# Build the project
echo -e "${YELLOW}Building project...${NC}"
make -j$(nproc)

# Resolve actual DLL output path (CMake may place it in bin/ or lib/)
BUILT_DLL=""
for candidate in "bin/$OUTPUT_DLL" "lib/$OUTPUT_DLL" "$OUTPUT_DLL" "bin/${PROJECT_NAME}.dll" "lib/${PROJECT_NAME}.dll" "${PROJECT_NAME}.dll"; do
    if [ -f "$candidate" ]; then
        BUILT_DLL="$candidate"
        break
    fi
done

# Check if build was successful
if [ -z "$BUILT_DLL" ]; then
    echo -e "${RED}Error: Build failed! DLL not found in expected output locations.${NC}"
    echo "Checked: bin/$OUTPUT_DLL, lib/$OUTPUT_DLL, $OUTPUT_DLL"
    exit 1
fi

echo -e "${GREEN}Build successful! Created: $BUILT_DLL${NC}"

# Check if Northstar directory exists
if [ ! -d "$NORTHSTAR_PLUGINS_DIR" ]; then
    echo -e "${YELLOW}Warning: Northstar plugins directory not found at:${NC}"
    echo "  $NORTHSTAR_PLUGINS_DIR"
    echo -e "${YELLOW}Creating directory...${NC}"
    mkdir -p "$NORTHSTAR_PLUGINS_DIR"
fi

# Copy the DLL to Northstar plugins directory
echo -e "${YELLOW}Copying DLL to Northstar plugins directory...${NC}"
cp "$BUILT_DLL" "$NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL"

# Verify the copy
if [ -f "$NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL" ]; then
    echo -e "${GREEN}Success! Plugin installed to:${NC}"
    echo "  $NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL"
    
    # Show file info
    echo -e "${BLUE}File details:${NC}"
    ls -lh "$NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL"
    file "$NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL"
else
    echo -e "${RED}Error: Failed to copy DLL to Northstar directory!${NC}"
    exit 1
fi

echo -e "${GREEN}=== Build and installation complete! ===${NC}"
echo -e "${BLUE}The plugin is now ready to use with Northstar.${NC}"
