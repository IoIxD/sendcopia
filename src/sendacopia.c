#include <Windows.h>
#include <MinHook.h>
#include <stdio.h>

#define SE_LOG(...) do {char msg[255]; snprintf(msg, sizeof(msg), "SENDACOPIA: " __VA_ARGS__); OutputDebugString(msg);} while(0);

BOOL(WINAPI* fpCreateDirectoryW)(
    LPCWSTR                lpPathName,
    LPSECURITY_ATTRIBUTES  lpSecurityAttributes
    ) = NULL;

BOOL WINAPI CreateDirectoryW_Hook(
    LPCWSTR                lpPathName,
    LPSECURITY_ATTRIBUTES  lpSecurityAttributes)
{
    SE_LOG("CreateDirectoryW called : % ws\n", lpPathName ? lpPathName : L"(null)");

    BOOL result = fpCreateDirectoryW(lpPathName, lpSecurityAttributes);

    SE_LOG("Result: %d (GetLastError: %lu)\n", result, GetLastError());

    return result;
}

void InstallHook()
{
    if (MH_Initialize() != MH_OK) {
        SE_LOG("MH_Initialize failed!\n");
        return;
    }

    // this is a kernel32 function but on my system (Windows 11) it forwards to kernelbase.dll, presumably it does this
    // on all platforms where this is supported? idk
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!hKernelBase) {
        SE_LOG("GetModuleHandleW failed!\n");
        return;
    }

    LPVOID pTarget = (LPVOID)GetProcAddress(hKernelBase, "CreateDirectoryW");
    if (!pTarget) {
        SE_LOG("GetProcAddress failed!\n");
        return;
    }

    if (MH_CreateHook(pTarget,
        &CreateDirectoryW_Hook,
        (LPVOID*)(&fpCreateDirectoryW)) != MH_OK) {
        SE_LOG("MH_CreateHook failed!\n");
        return;
    }

    if (MH_EnableHook(pTarget) != MH_OK) {
        SE_LOG("MH_EnableHook failed!\n");
        return;
    }

    SE_LOG("InstallHook success!\n");
}

static void RemoveHook()
{
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) return;

    LPVOID target = (LPVOID)GetProcAddress(hKernel32, "GetFullPathNameW");
    if (target)
    {
        MH_DisableHook(target);
        MH_RemoveHook(target);
    }
    MH_Uninitialize();
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD fdwReason, LPVOID lpvReserved) {
    SE_LOG("DllMain called %d\n", fdwReason);

    switch (fdwReason)
    {
    case DLL_THREAD_ATTACH:
        DisableThreadLibraryCalls(hModule);
        InstallHook();
        break;
    case DLL_THREAD_DETACH:
        //RemoveHook();
        break;
    }
    return TRUE;
}