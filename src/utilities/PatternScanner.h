#pragma once

#include "../pch.h"

class PatternScanner {
public:
    // Find a signature in a loaded module by name (e.g. "client.dll").
    static uintptr_t FindPattern(const char* moduleName, const char* pattern);

    // Find a signature in a loaded module by handle.
    static uintptr_t FindPattern(HMODULE module, const char* pattern);

    // Resolve an RIP-relative operand target from an instruction address.
    static uintptr_t ResolveRipRelative(uintptr_t instructionAddress, int instructionSize, int displacementOffset = 3);

private:
    static std::vector<int> PatternToBytes(const char* pattern);
};
