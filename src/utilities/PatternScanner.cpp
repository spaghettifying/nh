#include "PatternScanner.h"

#include <cctype>

std::vector<int> PatternScanner::PatternToBytes(const char* pattern) {
    std::vector<int> bytes;
    if (!pattern) {
        return bytes;
    }

    const char* cur = pattern;
    while (*cur) {
        if (*cur == ' ') {
            ++cur;
            continue;
        }

        if (*cur == '?') {
            ++cur;
            if (*cur == '?') {
                ++cur;
            }
            bytes.push_back(-1);
            continue;
        }

        if (std::isxdigit(static_cast<unsigned char>(cur[0])) &&
            std::isxdigit(static_cast<unsigned char>(cur[1]))) {
            const char hex[3] = {cur[0], cur[1], '\0'};
            bytes.push_back(static_cast<int>(std::strtoul(hex, nullptr, 16)));
            cur += 2;
            continue;
        }

        ++cur;
    }

    return bytes;
}

uintptr_t PatternScanner::FindPattern(const char* moduleName, const char* pattern) {
    if (!moduleName) {
        return 0;
    }

    HMODULE module = GetModuleHandleA(moduleName);
    return FindPattern(module, pattern);
}

uintptr_t PatternScanner::FindPattern(HMODULE module, const char* pattern) {
    if (!module || !pattern) {
        return 0;
    }

    auto* base = reinterpret_cast<unsigned char*>(module);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (!dos || dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return 0;
    }

    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (!nt || nt->Signature != IMAGE_NT_SIGNATURE) {
        return 0;
    }

    const size_t moduleSize = nt->OptionalHeader.SizeOfImage;
    const auto bytes = PatternToBytes(pattern);
    if (bytes.empty() || moduleSize < bytes.size()) {
        return 0;
    }

    for (size_t i = 0; i <= moduleSize - bytes.size(); ++i) {
        bool matched = true;
        for (size_t j = 0; j < bytes.size(); ++j) {
            if (bytes[j] != -1 && base[i + j] != static_cast<unsigned char>(bytes[j])) {
                matched = false;
                break;
            }
        }

        if (matched) {
            return reinterpret_cast<uintptr_t>(base + i);
        }
    }

    return 0;
}

uintptr_t PatternScanner::ResolveRipRelative(uintptr_t instructionAddress, int instructionSize, int displacementOffset) {
    if (!instructionAddress) {
        return 0;
    }

    const int32_t rel = *reinterpret_cast<int32_t*>(instructionAddress + displacementOffset);
    return instructionAddress + instructionSize + rel;
}
