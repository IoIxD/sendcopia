#include <Windows.h>
#include <mciapi.h>
#include <stdio.h>
#include <assert.h>

#define FUNC_DEF(x) static FARPROC _##x = NULL
FUNC_DEF(mciGetErrorStringA);
FUNC_DEF(mciSendStringA);
FUNC_DEF(timeBeginPeriod);
FUNC_DEF(timeEndPeriod);
#undef FUNC_DEF

#define FUNC_DEF(name) __declspec(dllexport) MMRESULT __stdcall name##() {return _##name();}
    FUNC_DEF(mciGetErrorStringA);
    FUNC_DEF(mciSendStringA);
    FUNC_DEF(timeBeginPeriod);
    FUNC_DEF(timeEndPeriod);
#undef FUNC_DEF


BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    MessageBox(NULL, "Test", "test", MB_OK);

    if (fdwReason == DLL_PROCESS_ATTACH) {
        char winmmPath[MAX_PATH] = { 0 };
        GetSystemDirectory(winmmPath, MAX_PATH);
        strcat_s(winmmPath, MAX_PATH, "\\winmm.dll");
        HMODULE dll = LoadLibraryA(winmmPath);
        if (dll) {
            #define FUNC_DEF(x) _##x = GetProcAddress(dll, #x)
            FUNC_DEF(mciGetErrorStringA);
            FUNC_DEF(mciSendStringA);
            FUNC_DEF(timeBeginPeriod);
            FUNC_DEF(timeEndPeriod);
            #undef FUNC_DEF
        }
        else {
            MessageBox(NULL, "System version of winmm.dll not found.", "fucked up system setup", MB_OK);
            return FALSE;
        }
    }

    return TRUE;
}
