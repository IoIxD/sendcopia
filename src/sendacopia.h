#pragma once 
#include <Windows.h>

#define SE_LOG(...) do {char msg[255]; snprintf(msg, sizeof(msg), "SENDACOPIA: " __VA_ARGS__); OutputDebugString(msg);} while(0);

void SendacopiaShowWindowsError();

const char* SendacopiaGetChoice();
void SendacopiaSetChoice(char* path);

#define SENDACOPIA_DEFAULT_SAVE_TEXT "<default>"
#define SENDACOPIA_NO_SAVE_TEXT "<no save>"