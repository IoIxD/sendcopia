#include <Windows.h>

static void SendacopiaInstallHooks();

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        SendacopiaInstallHooks();
    }

    return TRUE;
}

static void SendacopiaInstallHooks() {
    
};
