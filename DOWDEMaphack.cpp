#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include <Psapi.h>
#include <conio.h>
#include <fstream>

uintptr_t GetBaseAddress(HANDLE hProcess) {
    if (hProcess == NULL)
        return 0;
    HMODULE lphModule[1024];
    DWORD lpcbNeeded = 0;
    if (!EnumProcessModules(hProcess, lphModule, sizeof(lphModule), &lpcbNeeded))
        return 0;
    return reinterpret_cast<uintptr_t>(lphModule[0]);
}

template <typename T>
T ReadMemory(HANDLE hProcess, uintptr_t address) {
    T value;
    ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), nullptr);
    return value;
}

template <typename T>
bool WriteMemory(HANDLE hProcess, uintptr_t address, const T& value) {
    return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(address), &value, sizeof(T), nullptr);
}

int toggleFOW() {
    HWND hWnd = FindWindow(NULL, L"Warhammer 40,000: Dawn of War");
    if (hWnd == nullptr) {
        std::cout << "Hwnd error" << std::endl;
        return 1;
    }
    DWORD PID = 0;
    GetWindowThreadProcessId(hWnd, &PID);
    if (PID == 0) {
        std::cout << "Pid error" << std::endl;
        return 1;
    }
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, PID);
    if (hProcess == nullptr) {
        std::cout << "Open process error" << std::endl;
        return 1;
    }
    uintptr_t imageBase = GetBaseAddress(hProcess);
    uintptr_t world = ReadMemory<uintptr_t>(hProcess , imageBase + 0x1347880);
    uintptr_t terrain = ReadMemory<uintptr_t>(hProcess, world + 0x40);
    BYTE fow = ReadMemory<BYTE>(hProcess, terrain + 0xc41);
    if (fow == 0x01) {
        fow = 0x00;
    }
    else if (fow == 0x00) {
        fow = 0x01;
    }
    WriteMemory<BYTE>(hProcess, terrain + 0xc41, fow);
    CloseHandle(hProcess);
    return 0;
}
int main() {
    std::cout << "Press F9" << std::endl;
    while (true) {
        if (GetAsyncKeyState(VK_F9) & 0x8000) {
            toggleFOW();
        }
        Sleep(100);
    }
    return 0;
}
