#include <Windows.h>
#include <MinHook.h>
#include <stdio.h>
#include "../gui/gui.h"
#include "../sendacopia.h"
#include <memory.h>
#include <Windows.h>
#include <ShlObj.h>
#include <objbase.h>
#include <DbgHelp.h>

static BOOL HasDoneInitialization = FALSE;

BOOL(WINAPI* fpSHGetSpecialFolderPathW)(
    HWND    hwnd,
    LPWSTR  pszPath,
    int     csidl,
    BOOL    fCreate
) = NULL;

void* (__fastcall *fpFUN_00560800)(void* param_1, LPCWSTR param_2) = NULL;

// Detour function
BOOL WINAPI SHGetSpecialFolderPathW_Hook(
    HWND    hwnd,
    LPWSTR  pszPath,
    int     csidl,
    BOOL    fCreate)
{
    SE_LOG("[HOOK] SHGetSpecialFolderPathW %s\n", pszPath);
    return fpSHGetSpecialFolderPathW(hwnd, pszPath, csidl, fCreate);
}

BOOL CALLED_00516c00_ONCE = FALSE;


/* 
    Function for checking some aspect of the same folder.
    If we return NULL, the game won't try to save anywhere.
*/
int FUN_00516c00_Hook(void) {
    if (strcmp(SendacopiaGetChoice(), SENDACOPIA_NO_SAVE_TEXT) == 0) {
        return NULL;
    } else {
        return 1;
    }
}

wchar_t modifiedSavePathWide[MAX_PATH];

/* 
    Function for checking for paths and such.
    We can hijack the save directory at this point
   */
void* __fastcall FUN_00560800_Hook(void* param_1, LPCWSTR param_2) {
    char* name = SendacopiaGetChoice();
    PWSTR path = NULL;
    HRESULT r = SHGetKnownFolderPath(&FOLDERID_SavedGames, KF_FLAG_CREATE, NULL, &path);

    SE_LOG("FUN_00560800, name: %s, param_2: %ws, path: %ws, wcscmp: %d\n", name, param_2, path, wcscmp(param_2, path));
    if (wcscmp(param_2, path) == 0) {
        SE_LOG("wide modifiedSavePath %ws\n", param_2);
        MultiByteToWideChar(CP_UTF8, 0, SendacopiaGetChoice(), -1, modifiedSavePathWide, MAX_PATH);
        param_2 = modifiedSavePathWide;
    }
    //if (strcmp(name, SENDACOPIA_DEFAULT_SAVE_TEXT) == 0 || strcmp(name, SENDACOPIA_NO_SAVE_TEXT) == 0) {
    //} else {

    //    if (wcscmp(param_2, path) == 0) {
    //        SE_LOG("FUN_00560800, save path %ws\n", modifiedSavePath);
    //        param_2 = modifiedSavePath;
    //    }
    //}

    return fpFUN_00560800(param_1, param_2);
};

char* FUN_00500620_Hook(void) {
    return TRUE;
}

void InstallHook() {
    if (MH_Initialize() != MH_OK) {
        SE_LOG("MH_Initialize failed!\n");
        return;
    }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        SE_LOG("GetModuleHandleW failed!\n");
        return;
    }

    LPVOID pTarget = (LPVOID)GetProcAddress(hShell32, "SHGetSpecialFolderPathW");
    if (!pTarget) {
        SE_LOG("GetProcAddress failed!\n");
        return;
    }

    //if (MH_CreateHook(pTarget,
    //    &SHGetSpecialFolderPathW_Hook,
    //    (LPVOID*)(&fpSHGetSpecialFolderPathW)) != MH_OK) {
    //    SE_LOG("MH_CreateHook failed!\n");
    //    return;
    //}

    if (MH_CreateHook((void*)0x00516c00,
        &FUN_00516c00_Hook,
        NULL) != MH_OK) {
        SE_LOG("MH_CreateHook failed!\n");
        return;
    }
    if (MH_CreateHook((void*)0x00500620,
        &FUN_00500620_Hook,
        NULL) != MH_OK) {
        SE_LOG("MH_CreateHook failed!\n");
        return;
    }
    if (MH_CreateHook((void*)0x00560800,
        &FUN_00560800_Hook,
        &fpFUN_00560800) != MH_OK) {
        SE_LOG("MH_CreateHook failed!\n");
        return;
    }
    

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        SE_LOG("MH_EnableHook failed!\n");
        return;
    }

    SE_LOG("InstallHook success!\n");
}

void RemoveHook()
{
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) return;

    LPVOID target = (LPVOID)GetProcAddress(hShell32, "SHGetSpecialFolderPathW");
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
    case DLL_PROCESS_ATTACH:
        if(!HasDoneInitialization) {
            SendacopiaGUIState * state = SendacopiaGUIStateNew();
            MwBool cont = SendacopiaGUIStateLoop(state);
            SendacopiaGUIStateFree(state);

            if (cont == MwFALSE) {
                DisableThreadLibraryCalls(hModule);
                InstallHook();
            }
            else {
                ExitProcess(0);
            }
     
            HasDoneInitialization = TRUE;
        }
        break;
    case DLL_PROCESS_DETACH:
        //RemoveHook();
        break;
    }
    return TRUE;
}