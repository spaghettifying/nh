#pragma once

#include "../pch.h"

namespace Memory {
    template <int VTableIdx, typename ReturnType, typename... Args>
    ReturnType CallVirtual(void* pClassInstance, Args... args) {
        if (pClassInstance) {
            void** fTable = *reinterpret_cast<void***>(pClassInstance);
            return reinterpret_cast<ReturnType(__thiscall*)(void*, decltype(args)...)>(
                fTable[VTableIdx])(pClassInstance, args...);
        }

        return ReturnType{};
    }

    template <typename ReturnType, typename... Args>
    ReturnType InvokeVTableFunction(void *pClassInstance, const int VTableIdx, Args... args) {
        if (pClassInstance) {
          void **fTable = *reinterpret_cast<void***>(pClassInstance);
          return reinterpret_cast<ReturnType(__thiscall *)(void *, decltype(args)...)>(
              fTable[VTableIdx])(pClassInstance, args...);
        }

        return ReturnType{};
    }
}