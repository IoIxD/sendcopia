#define _MCIAPI_H_
#define _MMEAPI_H_
#define _TIMERAPI_H_
#include <windows.h>
#include <mciapi.h>
#include <stdio.h>
#include <assert.h>

#define FUNC_DEFS \
    FUNC_DEF(mciGetErrorStringA);\
    FUNC_DEF(mciSendStringA);\
    FUNC_DEF(timeBeginPeriod);\
    FUNC_DEF(timeEndPeriod);\
    FUNC_DEF(timeGetDevCaps); \
    FUNC_DEF(waveInAddBuffer); \
    FUNC_DEF(waveInOpen); \
    FUNC_DEF(waveInClose); \
    FUNC_DEF(waveInGetDevCapsW); \
    FUNC_DEF(waveInGetNumDevs); \
    FUNC_DEF(waveInPrepareHeader); \
    FUNC_DEF(waveInUnprepareHeader); \
    FUNC_DEF(waveInStart); \
    FUNC_DEF(waveInReset); \
    FUNC_DEF(waveOutOpen); \
    FUNC_DEF(waveOutClose); \
    FUNC_DEF(waveOutGetDevCapsW); \
    FUNC_DEF(waveOutGetNumDevs); \
    FUNC_DEF(waveOutPrepareHeader); \
    FUNC_DEF(waveOutUnprepareHeader); \
    FUNC_DEF(waveOutStart); \
    FUNC_DEF(waveOutReset);  \
    FUNC_DEF(waveOutGetErrorTextW); \
    FUNC_DEF(waveOutWrite); \
    FUNC_DEF(mixerSetControlDetails); \
    FUNC_DEF(mixerSetControlDetailsA); \

#define FUNC_DEF(x) static FARPROC pfn_##x = NULL
    FUNC_DEFS
#undef FUNC_DEF

#define FUNC_DEF(name) __declspec(dllexport) MMRESULT _cdecl name() {return pfn_##name();}
    FUNC_DEFS
#undef FUNC_DEF


BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        OutputDebugString("WINMM INJECTION SUCCESS\n");
        char winmmPath[MAX_PATH] = { 0 };
        GetSystemDirectory(winmmPath, MAX_PATH);
        strcat_s(winmmPath, MAX_PATH, "\\winmm.dll");
        HMODULE dll = LoadLibraryA(winmmPath);
        if (dll) {
            #define FUNC_DEF(x) pfn_##x = GetProcAddress(dll, #x)
                FUNC_DEFS
            #undef FUNC_DEF
        }       else {
            MessageBox(NULL, "System version of winmm.dll not found.", "fucked up system setup", MB_OK);
            return FALSE;
        }
    
        HANDLE sendacopia = LoadLibrary("sendacopia.dll");
        if (sendacopia == NULL) {
            /* (duplication of the function in sendacopia.h) */
            DWORD errorMessageID = GetLastError();
            char fullMessageBuffer[2048];
            LPSTR messageBuffer = NULL;
            size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
            snprintf(fullMessageBuffer, sizeof(fullMessageBuffer), "Failed to load sendacopia.dll!\n%s(Do you also have Mw.dll in the directory?)\n", messageBuffer);
            MessageBox(NULL, fullMessageBuffer, "OOPS", MB_OK);
            LocalFree(messageBuffer);
            return FALSE;
        }
    }


    return TRUE;
}
