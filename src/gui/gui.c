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
		if (strcmp(text, SENDACOPIA_DEFAULT_SAVE_TEXT) == 0) {
			SendacopiaSetChoice(SENDACOPIA_DEFAULT_SAVE);
		} else if (strcmp(text, SENDACOPIA_NO_SAVE_TEXT) == 0) {
			SendacopiaSetChoice(SENDACOPIA_NO_SAVE);
		}
		else {
			int index = SendacopiaGetChoiceFromINI(text);
			SE_LOG("Opening %s (index %d)\n", text, index);
			SendacopiaSetChoice(index);
		}
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

	SendacopiaSetChoice(SENDACOPIA_DEFAULT_SAVE);
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

static int replacechar(char* str, char orig, char rep) {
	char* ix = str;
	int n = 0;
	while ((ix = strchr(ix, orig)) != NULL) {
		*ix++ = rep;
		n++;
	}
	return n;
}
void SendacopiaGUIStatePopulateList(SendacopiaGUIState* state) {
	char curdir[MAX_PATH] = { 0 };
	char saveloc[MAX_PATH] = { 0 };
	char buffer[8096] = { 0 };
	GetModuleFileName(NULL, curdir, MAX_PATH);
	snprintf(saveloc, MAX_PATH, "%s\\..\\sendacopia_saves.ini", curdir);
	SE_LOG("opening %s\n", saveloc);

	GetPrivateProfileSectionA("Saves", buffer, sizeof(buffer), saveloc);

	MwListBoxReset(state->fileChooseListBox);
	MwListBoxSet(state->fileChooseListBox, -1, -1, SENDACOPIA_DEFAULT_SAVE_TEXT);
	MwListBoxSet(state->fileChooseListBox, -1, -1, SENDACOPIA_NO_SAVE_TEXT);

	char* p = buffer;
	SE_LOG("now iterating\n");
	while (*p) {
		char key[2048] = { 0 };
		memset(key, 0, 2048);
		snprintf(key, 2047, "%s", p);
		replacechar(key, '=', '\0');
		MwListBoxSet(state->fileChooseListBox, -1, -1, key);
		p += strlen(p) + 1;
	}
};

