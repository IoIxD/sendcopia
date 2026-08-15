#include "sendacopia.h"
#include "json.h"

static int CHOICE;

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

int SendacopiaGetChoice() {
    return CHOICE;
};
void SendacopiaSetChoice(int path) {
    CHOICE = path;
};

void SendacopiaNewFile(const char* filename) {
    char curdir[MAX_PATH] = { 0 };
    char saveloc[MAX_PATH] = { 0 };
    char buffer[2048] = { 0 };
    int index = 0;

    GetModuleFileName(NULL, curdir, MAX_PATH);
    snprintf(saveloc, MAX_PATH, "%s\\..\\sendacopia_saves.ini",curdir);

    GetPrivateProfileSectionA("Saves", buffer, sizeof(buffer), saveloc);

    char* p = buffer;
    SE_LOG("now iterating\n");
    while (*p) {
        SE_LOG("Found %s\n", p);
        p += strlen(p) + 1;
        ++index;
    }
    index += 10;
    if (index == 999) {
        index = 1000;
    }
    SE_LOG("index %d\n", index);

    char indbuffer[2048] = { 0 };
    snprintf(indbuffer, sizeof(indbuffer) - 1, "%d", index);

    SE_LOG("SAVING %d to %s AT %s\n", index, filename, saveloc);
    WritePrivateProfileStringA("Saves", filename, indbuffer, saveloc);
};

int SendacopiaGetChoiceFromINI(char * filename) {
    char curdir[MAX_PATH] = { 0 };
    char saveloc[MAX_PATH] = { 0 };
    char buffer[2048] = { 0 };
    int index = 0;

    GetModuleFileName(NULL, curdir, MAX_PATH);
    snprintf(saveloc, MAX_PATH, "%s\\..\\sendacopia_saves.ini", curdir);

    GetPrivateProfileStringA("Saves", filename, "999", buffer, sizeof(buffer), saveloc);

    /* this function is unsafe but for now idgaf we'll swap it for something better later */
    return atoi(buffer);
}