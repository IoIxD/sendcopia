#pragma once 
#include <Windows.h>

#define SE_LOG(...) do {char msg[255]; snprintf(msg, sizeof(msg), "SENDACOPIA: " __VA_ARGS__); OutputDebugString(msg);} while(0);
#define SE_NOTIFY(...) do {char msg[255]; snprintf(msg, sizeof(msg), "SENDACOPIA: " __VA_ARGS__); MessageBox(NULL, msg, "Notice", MB_OK);} while(0);

void SendacopiaShowWindowsError(const char * prefix);

int SendacopiaGetChoice();
void SendacopiaSetChoice(int path);
int SendacopiaGetChoiceFromINI(char * filename);

void SendacopiaNewFile(const char* filename);

#define SENDACOPIA_DEFAULT_SAVE_TEXT "<default>"
#define SENDACOPIA_NO_SAVE_TEXT "<no save>"
#define SENDACOPIA_DEFAULT_SAVE 2
#define SENDACOPIA_NO_SAVE -1