#!/bin/bash

# NSCppPluginTemplate Clean Script
# Removes build artifacts and the plugin from Northstar directory

# Configuration
PROJECT_NAME="nh"
BUILD_DIR="build"
BUILD_DIR_ALT="build"
NORTHSTAR_PLUGINS_DIR="$HOME/.local/share/Steam/steamapps/common/Titanfall2/R2Northstar/plugins"
OUTPUT_DLL="${PROJECT_NAME}.dll"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== nh Clean Script ===${NC}"

# Remove build directory
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Removing build directory: $BUILD_DIR${NC}"
    rm -rf "$BUILD_DIR"
    echo -e "${GREEN}Build directory removed.${NC}"
else
    echo -e "${YELLOW}Build directory not found.${NC}"
fi

# Remove alternate build directory
if [ -d "$BUILD_DIR_ALT" ]; then
    echo -e "${YELLOW}Removing build directory: $BUILD_DIR_ALT${NC}"
    rm -rf "$BUILD_DIR_ALT"
    echo -e "${GREEN}Build directory removed.${NC}"
else
    echo -e "${YELLOW}Build directory not found.${NC}"
fi

# Remove local runtime logs (can contain machine paths and profile info)
if ls log.txt nslog*.txt >/dev/null 2>&1; then
    echo -e "${YELLOW}Removing local runtime logs...${NC}"
    rm -f log.txt nslog*.txt
    echo -e "${GREEN}Local logs removed.${NC}"
else
    echo -e "${YELLOW}No local runtime logs found.${NC}"
fi

# Remove plugin from Northstar directory
if [ -f "$NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL" ]; then
    echo -e "${YELLOW}Removing plugin from Northstar: $OUTPUT_DLL${NC}"
    rm -f "$NORTHSTAR_PLUGINS_DIR/$OUTPUT_DLL"
    echo -e "${GREEN}Plugin removed from Northstar.${NC}"
else
    echo -e "${YELLOW}Plugin not found in Northstar directory.${NC}"
fi

echo -e "${GREEN}=== Clean complete! ===${NC}"
