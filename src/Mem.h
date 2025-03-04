#pragma once

#include <vector>
#include <Windows.h>
#include <TlHelp32.h>

class Mem
{
private:
    DWORD processID;
    HANDLE processHandle;

public:
    Mem(const wchar_t* processName);
    ~Mem();

    DWORD GetProcessID();
    HANDLE GetProcessHandle();
    uintptr_t GetModuleAddress(const wchar_t* moduleName);
    HMODULE GetModuleHandleW(const wchar_t* moduleName);
    bool InForeground();

    template <typename T>
    T ReadMemory(uintptr_t address)
    {
        T buffer;
        ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, sizeof(T), NULL);
        return buffer;
    }

    template <typename T>
    void ReadMemory(uintptr_t address, T* buffer, size_t size)
    {
        ReadProcessMemory(processHandle, (LPCVOID)address, buffer, size * sizeof(T), NULL);
    }

    template <typename T>
    void WriteMemory(uintptr_t address, T value)
    {
        WriteProcessMemory(processHandle, (LPVOID)address, &value, sizeof(T), NULL);
    }

    template <typename T>
    T ReadPointerChain(uintptr_t baseAddress, const uintptr_t* offsets, size_t numOffsets)
    {
        uintptr_t currentAddress = baseAddress;

        for (size_t i = 0; i < numOffsets; ++i)
            currentAddress = ReadMemory<uintptr_t>(currentAddress + offsets[i]);

		return currentAddress;
    }
};
