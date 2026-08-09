# sendacopia

Mod for Endacopia that adds a save game manager to the game's launch. It also allows you to launch without being able to save at all for speedrunning purposes.

<img width="351" height="296" alt="image" src="https://github.com/user-attachments/assets/b3012122-dadf-4854-a5cc-dee88cfb966d" />

# Installation
Download the zip file and unzip the three DLLs given to you into Endacopia's installation directory


# Usage
A save chooser will popup upon launching the game (options other then the first two will not be there). You can select an option here and click Confirm to launch the game.

Choosing `<default>` will have you loading a save file from the default `Saved Games` location

Choosing `<no save>` will put the game in a state where it's unable to save; it'll launch with an error, and trying to click the save icons at any point will also put up an error.

Choosing on any other option will launch the game at the appropriate save file. You can add a new option using the "New File" button next to the Confirm button.
