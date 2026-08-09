#include "sendacopia.h"

static char CHOICE[MAX_PATH];

void SendacopiaShowWindowsError(const char * prefix) {
    // My favorite block of code from stackoverflow!
    // (Getting GetLastError as a string)
    DWORD errorMessageID = GetLastError();
    char fullMessageBuffer[2048];
    LPSTR messageBuffer = NULL;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    snprintf(fullMessageBuffer, sizeof(fullMessageBuffer), "%s\n%s\n", prefix, messageBuffer);
    MessageBox(NULL, fullMessageBuffer, "OOPS", MB_OK);
    LocalFree(messageBuffer);
}

const char* SendacopiaGetChoice() {
    return CHOICE;
};
void SendacopiaSetChoice(char* path) {
    snprintf(CHOICE, sizeof(CHOICE), path);
};