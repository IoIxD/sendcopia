#include "gui.h"
#include "../sendacopia.h"

#include <memory.h>
#include <Windows.h>
#include <ShlObj.h>
#include <objbase.h>

static MwBool dohardclose = MwFALSE;
static MwBool running = MwTRUE;

static void confirm(MwWidget handle, void* user, void* call) {
	SendacopiaGUIState* state = user;
	int idx = MwGetInteger(state->fileChooseListBox, MwNvalue);
	char* text = MwListBoxGet(state->fileChooseListBox, idx);
	if (text) {
		SE_LOG("Opening %s\n", text);
		SendacopiaSetChoice(text);
	}
	running = MwFALSE;
}

static void close(MwWidget handle, void* user, void* call) {
	dohardclose = MwTRUE;
}

SendacopiaGUIState* SendacopiaGUIStateNew() {
	MwLibraryInit();
	CoInitialize(NULL);

	dohardclose = MwFALSE;

	SendacopiaGUIState* state = malloc(sizeof(SendacopiaGUIState));
	if (!state) {
		return NULL;
	}
	memset(state, 0, sizeof(state));

	state->mutex = CreateMutex(NULL, TRUE, NULL);

	state->window = MwVaCreateWidget(MwWindowClass, NULL, NULL, MwDEFAULT, MwDEFAULT, 320, 240, MwNtitle, "Endacopia Save Manager\n", NULL);
	state->mainBox = MwVaCreateWidget(MwBoxClass, NULL, state->window, 0, 0, 320, 240, MwNorientation, MwVERTICAL, NULL);

	state->fileChooserBox = MwVaCreateWidget(MwBoxClass, NULL, state->mainBox, 0, 0, 320, 240, MwNorientation, MwVERTICAL, MwNratio, 6, NULL);
	state->fileChooseListBox = MwCreateWidget(MwListBoxClass, NULL, state->fileChooserBox, 0, 0, 320, 240);

	SendacopiaGUIStatePopulateList(state);

	state->buttonBox = MwVaCreateWidget(MwBoxClass, NULL, state->mainBox, 0, 0, 320, 240, MwNorientation, MwHORIZONTAL, NULL);
	state->newFileButton = MwVaCreateWidget(MwButtonClass, NULL, state->buttonBox, 0, 0, 1, 1, MwNtext, "New File", NULL);
	state->confirmButton = MwVaCreateWidget(MwButtonClass, NULL, state->buttonBox, 0, 0, 1, 1, MwNtext, "Confirm", NULL);

	MwAddUserHandler(state->newFileButton, MwNactivateHandler, SendacopiaGUIState_NewFileHandler, state);

	MwAddUserHandler(state->confirmButton, MwNactivateHandler, confirm, state);
	MwAddUserHandler(state->window, MwNcloseHandler, close, state);

	state->filenameChooserCtx.window = NULL;

	SendacopiaSetChoice(SENDACOPIA_DEFAULT_SAVE_TEXT);
	return state;
};
MwBool SendacopiaGUIStateLoop(SendacopiaGUIState* state) {
	while (!MwWindowShouldClose(state->window) && running) {
		WaitForSingleObject(state->mutex,INFINITE); 
		MwStep(state->window);
		if (state->filenameChooserCtx.window) {
			MwStep(state->filenameChooserCtx.window);
		}
		ReleaseMutex(state->mutex);
	}

	return dohardclose;
};
void SendacopiaGUIStateFree(SendacopiaGUIState * state) {
	if(state->filenameChooserCtx.window) MwShow(state->filenameChooserCtx.window, MwFALSE);

	MwShow(state->window, MwFALSE);
	MwStep(state->window);

	MwDestroyWidget(state->fileChooseListBox);
	MwDestroyWidget(state->fileChooserBox);
	MwDestroyWidget(state->mainBox);
	MwDestroyWidget(state->window);

	free(state);
};
void SendacopiaGUIStatePopulateList(SendacopiaGUIState* state) {
	MwListBoxReset(state->fileChooseListBox);

	MwListBoxSet(state->fileChooseListBox, -1, -1, SENDACOPIA_DEFAULT_SAVE_TEXT);
	MwListBoxSet(state->fileChooseListBox, -1, -1, SENDACOPIA_NO_SAVE_TEXT);

	PWSTR path = NULL;
	char fullpath[MAX_PATH];
	wchar_t wpath[MAX_PATH * 4];
	HRESULT r = SHGetKnownFolderPath(&FOLDERID_SavedGames, KF_FLAG_CREATE, NULL, &path);
	if (r == S_OK)
	{
		snprintf(fullpath, sizeof(fullpath), "%ws\\Sendacopia\\*", path);
		MultiByteToWideChar(CP_UTF8, 0, fullpath, -1, wpath, sizeof(wpath));
		CreateDirectoryW(wpath, 0);
		SE_LOG("Using %ws for extra save games\n", wpath);

		WIN32_FIND_DATAW findData;
		HANDLE hFind = FindFirstFileW(wpath, &findData);

		if (hFind == INVALID_HANDLE_VALUE) {
			SE_LOG("Failed to open %ws; %d\n", wpath, GetLastError());
			return;
		}

		do {
			if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0) {
				continue;
			}

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				char folderName[MAX_PATH];
				WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, folderName, sizeof(folderName), 0, 0);
				SE_LOG("found %s\n", folderName);
				MwListBoxSet(state->fileChooseListBox, -1, -1, folderName);
			}
		} while (FindNextFileW(hFind, &findData));

		CoTaskMemFree(path);
	}
	else {
		SE_LOG("SHGetKnownFolderPath failed! %d\n", r);
	}

	
};

void SendacopiaGUIStateNewFile(SendacopiaGUIState* state, const char* filename) {
	PWSTR path = NULL;
	char fullpath[MAX_PATH];
	char err[MAX_PATH];
	HRESULT r = SHGetKnownFolderPath(&FOLDERID_SavedGames, KF_FLAG_CREATE, NULL, &path);
	if (r == S_OK)
	{
		snprintf(fullpath, sizeof(fullpath), "%ws\\Sendacopia\\%s", path, filename);
		if (!CreateDirectory(fullpath, 0)) {
			snprintf(err, sizeof(err), "Failed to create/open %s", fullpath);
			SendacopiaShowWindowsError(err);
		}
	}
};
