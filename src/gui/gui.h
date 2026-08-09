#pragma once

#include <Mw/Milsko.h>
#include <Windows.h>

typedef struct {
	MwWidget window;

	MwWidget mainBox;
	MwWidget fileChooserBox;
	MwWidget fileChooseListBox;

	MwWidget buttonBox;
	MwWidget newFileButton;
	MwWidget confirmButton;

	struct {
		MwWidget window;
		MwWidget vbox;
		MwWidget hbox;
		MwWidget entry;
		MwWidget confirm;
		MwWidget cancel;
	} filenameChooserCtx;

	HANDLE mutex;
	
} SendacopiaGUIState;

SendacopiaGUIState* SendacopiaGUIStateNew();
void SendacopiaGUIStatePopulateList(SendacopiaGUIState* state);
void SendacopiaGUIStateNewFile(SendacopiaGUIState* state, const char * filename);
MwBool SendacopiaGUIStateLoop(SendacopiaGUIState* state);
void SendacopiaGUIStateFree(SendacopiaGUIState* state);

void SendacopiaGUIState_NewFileHandler(MwWidget handle, void* user, void* call);