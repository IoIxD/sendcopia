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

void* (__fastcall *fpFUN_00560800)(void* param_1, LPCWSTR param_2) = NULL;

void (*fpSaveGame)(int slot, const char* descript) = NULL;
void (*fpRestoreGame)(int slot) = NULL;
//void *(__fastcall *fp_get_save_game_filename)(int slot) = NULL;
BOOL(__fastcall* fpTryRestoreSave)(void* path, int slot) = NULL;
BOOL (__fastcall *fpTryRestoreSave_NoPath)(int slot) = NULL;
void (*fpMain)(void);

/*
    Function for checking whether the saved games directory is valid.
    Naturally, if we return false, the game won't try to save anywhere.

    REMINDER TO SELF: I found this by
    - searching for uses of SHGetSpecialFolderPathW in Ghidra
    - finding a function with something like "FUN_0053ff60("My Saved Games",0xffffffff);" in it
    - hijacking the function that gets called above "if ((char)iVar2 == '\0')"
*/
int Win32SavedGamesDirectoryIsValid_Hook(void) {
    if (SendacopiaGetChoice() == SENDACOPIA_NO_SAVE) {
        return 0;
    }
    else {
        return 1;
    }
}

/*
    Function where the game tries to verify if it can save to a directory.
    We just return true.
    REMINDER TO SELF:
    I found this by searching for a string like "Unable to write in the savegame directory" and then finding which function call results in that.
*/
BOOL FUN_00500620_Hook(void) {
    return TRUE;
}


// FUN_00441bc0
BOOL __fastcall TryRestoreSave_NoPath_Hook(int slot) {
    SE_LOG("TryRestoreSave_NoPath (%d)\n", slot);
    if (slot == SENDACOPIA_DEFAULT_SAVE) {
        SE_LOG("attempting to override with %d\n", SendacopiaGetChoice());
        return fpTryRestoreSave_NoPath(SendacopiaGetChoice());
    }
    else {
        return fpTryRestoreSave_NoPath(slot);
    }
}


// FUN_00441c40
BOOL __fastcall TryRestoreSave_Hook(void* path, int slot) {
    SE_LOG("TryRestoreSave (%d)\n", slot);
    if (slot == SENDACOPIA_DEFAULT_SAVE) {
        SE_LOG("attempting to override with %d\n", SendacopiaGetChoice());
        return fpTryRestoreSave(path, SendacopiaGetChoice());
    }
    else {
        return fpTryRestoreSave(path, slot);
    }
}

// FUN_00440d70
void SaveGame_Hook(int slot, const char * descript) {
    SE_LOG("SaveGame (%d %s)\n", slot, descript);
    if (slot == SENDACOPIA_DEFAULT_SAVE) {
        SE_LOG("attempting to override with %d\n", SendacopiaGetChoice());
        fpSaveGame(SendacopiaGetChoice(), descript);
    }
    else {
        fpSaveGame(slot, descript);
    }
}



// FUN_0046a410
void RestoreGame_Hook(int slot) {
    SE_LOG("RestoreGameSlot (%d)\n", slot);
    if (slot == SENDACOPIA_DEFAULT_SAVE) {
        SE_LOG("attempting to override with %d\n", SendacopiaGetChoice());
        fpRestoreGame(SendacopiaGetChoice());
    }
    else {
        fpRestoreGame(slot);
    }
}

void Entry_Hook(void) {
    SE_LOG("Entry_Hook!\n");
    SendacopiaGUIState* state = SendacopiaGUIStateNew();
    MwBool cont = SendacopiaGUIStateLoop(state);
    SendacopiaGUIStateFree(state);

    if (cont == MwFALSE) {
        fpMain();
        SE_LOG("Continuing!\n");
    }
    else {
        SE_LOG("Not Continuing!\n");
    }
}

void InstallHook() {
    if (MH_Initialize() != MH_OK) {
        SE_LOG("MH_Initialize failed!\n");
        return;
    }

    struct {
        LPVOID target;
        LPVOID detour;
        LPVOID original;
    } funcs[] = {
        {(void*)0x00516c00, &Win32SavedGamesDirectoryIsValid_Hook, NULL},
        {(void*)0x0062B3C5, &Entry_Hook, &fpMain},
        {(void*)0x00440d70, &SaveGame_Hook, &fpSaveGame},
        {(void*)0x0046a410, &RestoreGame_Hook, &fpRestoreGame},
        {(void*)0x00441c40, &TryRestoreSave_Hook, &fpTryRestoreSave},
        {(void*)0x00441bc0, &TryRestoreSave_NoPath_Hook, &fpTryRestoreSave_NoPath},
        {(void*)0x00500620, FUN_00500620_Hook, NULL},
    };
    for (int i = 0; i < sizeof(funcs) / sizeof(*funcs); i++) {
        SE_LOG("MH_CreateHook for function entry %d...", i);
        if (MH_CreateHook(funcs[i].target, funcs[i].detour, funcs[i].original)) {
            OutputDebugString("FAILED!\n");
            return;
        } else {
            OutputDebugString("SUCCESS!\n");
        }
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        SE_LOG("MH_EnableHook failed!\n");
        return;
    }

    SE_LOG("InstallHook success!\n");
}

void RemoveHook()
{
    //HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    //if (!hShell32) return;
  
    MH_Uninitialize();
}


BOOL WINAPI DllMain(HINSTANCE hModule, DWORD fdwReason, LPVOID lpvReserved) {
    SE_LOG("DllMain called %d\n", fdwReason);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        InstallHook();
        break;
    case DLL_PROCESS_DETACH:
        //RemoveHook();
        break;
    }
    return TRUE;
}