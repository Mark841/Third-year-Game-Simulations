#include "../../Common/Window.h"
#include "FruitWizardGame.h"

using namespace NCL;
using namespace CSC3222;

int main() {
	Window*w = Window::CreateGameWindow("CSC3222 - Fruit Wizard!", 1120, 768);

	// If there was a problem initialising the game window
	if (!w->HasInitialised()) {
		return -1;
	}

	// Create new game object
	FruitWizardGame* theGame = new FruitWizardGame();
	w->GetTimer()->GetTimeDeltaSeconds(); //Reset so we don't get a big dt in the first frame!

	// While Escape is not pressed
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		// Get time between frames
		float time = w->GetTimer()->GetTimeDeltaSeconds();

		if (time > 1.0f) {
			continue; //Skip the timestep if it's huge!
		}

		// Update the game
		theGame->Update(time);

		// Show the console window if PG UP key is pressed
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		// Hide the console window if PG DOWN key is pressed
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}
		// Set fullscreen if Home key is pressed
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::HOME)) {
			w->SetFullScreen(true);
		}
		// Set windowed if End key is pressed
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::END)) {
			w->SetFullScreen(false);
		}

		// Update the window title to the time increment between frames
		w->SetTitle(std::to_string(time));

		// If all lives are lost restart the game
		if (theGame->GetLives() == 0)
		{
			theGame = new FruitWizardGame();
		}
	}

	// Delete the game
	delete theGame;
	Window::DestroyGameWindow();
}