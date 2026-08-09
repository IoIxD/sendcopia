#include "gui.h"
#include "../sendacopia.h"

static void destroy_picker(SendacopiaGUIState* state) {
	MwShow(state->filenameChooserCtx.window, MwFALSE);
	MwStep(state->filenameChooserCtx.window);
	MwDestroyWidget(state->filenameChooserCtx.window);

	memset(&state->filenameChooserCtx, 0, sizeof(state->filenameChooserCtx));
}
static void confirm(MwWidget handle, void* user, void* call) {
	SendacopiaGUIState* state = (SendacopiaGUIState*)user;
	WaitForSingleObject(state->mutex, INFINITE);
	SendacopiaGUIStateNewFile(state, MwGetText(state->filenameChooserCtx.entry, MwNtext));

	destroy_picker(state);
	ReleaseMutex(state->mutex);
}
static void cancel(MwWidget handle, void* user, void* call) {
	SendacopiaGUIState* state = (SendacopiaGUIState*)user;
	WaitForSingleObject(state->mutex, INFINITE);
	destroy_picker(state);
	ReleaseMutex(state->mutex);
}

void SendacopiaGUIState_NewFileHandler(MwWidget handle, void* user, void* call) {
	SendacopiaGUIState * state = (SendacopiaGUIState*)user;
	WaitForSingleObject(state->mutex, INFINITE);
	state->filenameChooserCtx.window = MwVaCreateWidget(MwWindowClass, NULL, NULL, MwDEFAULT, MwDEFAULT, 320, 100, MwNtitle, "New filename", NULL);
	state->filenameChooserCtx.vbox = MwVaCreateWidget(MwBoxClass, NULL, state->filenameChooserCtx.window, 0, 0, 320, 100, MwNorientation, MwVERTICAL, NULL);
	state->filenameChooserCtx.entry = MwVaCreateWidget(MwEntryClass, NULL, state->filenameChooserCtx.vbox, 0, 0, 1, 1, NULL);
	state->filenameChooserCtx.hbox = MwVaCreateWidget(MwBoxClass, NULL, state->filenameChooserCtx.vbox, 0,0,320,100, MwNorientation, MwVERTICAL, NULL);
	state->filenameChooserCtx.confirm = MwVaCreateWidget(MwButtonClass, NULL, state->filenameChooserCtx.hbox, 0, 0, 1, 1, MwNtext, "Confirm", NULL);
	state->filenameChooserCtx.cancel = MwVaCreateWidget(MwButtonClass, NULL, state->filenameChooserCtx.hbox, 0, 0, 1, 1, MwNtext, "Cancel", NULL);

	MwAddUserHandler(state->filenameChooserCtx.confirm, MwNactivateHandler, confirm, state);
	MwAddUserHandler(state->filenameChooserCtx.cancel, MwNactivateHandler, cancel, state);
	ReleaseMutex(state->mutex);
};