#include "GamePushdownState.h"

using namespace NCL::CSC8503;
#ifdef _x64
MainMenuState::MainMenuState(CourseworkGame* tg, RendererBase* r) {
	this->renderer = (OGLRenderer*) r;
	this->game = tg;
}

PushdownState::PushdownResult MainMenuState::OnUpdate(float dt, PushdownState** newState) {
	NCLVector4 selectedColour = Debug::GREEN;
	NCLVector4 defaultColour = Debug::WHITE;
	if (game->GetCurrentLevel() != 0) {
		// A game has been initialised, so provide option to resume
		minChoice = 0;
		maxChoice = 2;
		(currentChoice == 0) ? renderer->DrawString("Resume", NCLVector2(42.0f, 20.0f), selectedColour, 30.0f) :
			renderer->DrawString("Resume", NCLVector2(42.0f, 20.0f), defaultColour , 30.0f);
	}
	else {
		minChoice = 1;
		maxChoice = 2;
	}

	(currentChoice == 1) ? renderer->DrawString("Start Game", NCLVector2(35.0f, 50.0f), selectedColour, 30.0f) :
		renderer->DrawString("Start Game", NCLVector2(35.0f, 50.0f), defaultColour, 30.0f);

	(currentChoice == 2) ? renderer->DrawString("Quit", NCLVector2(45.0f, 80.0f), selectedColour, 30.0f) :
		renderer->DrawString("Quit", NCLVector2(45.0f, 80.0f), defaultColour, 30.0f);

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE) && game->GetCurrentLevel() != 0) {
		return PushdownResult::Pop;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP)) {
		currentChoice = (currentChoice > minChoice) ? (currentChoice - 1) : maxChoice;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN)) {
		currentChoice = (currentChoice < maxChoice) ? (currentChoice + 1) : minChoice;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) {
		if (currentChoice == 0 && game->GetCurrentLevel() != 0) {
			return PushdownResult::Pop;
		}
		else if (currentChoice == 1) {
			game->LoadLevel();
		}
		else if (currentChoice == 2) {
			exit(0);
		}
		return PushdownResult::Pop;
	}

	return PushdownResult::NoChange;
};

void MainMenuState::OnAwake() {
	// Deciding on which option the cursor starts, determined by whether the Resume option is being shown
	currentChoice = (game->GetCurrentLevel() == 0) ? 1 : 0;
}
#endif _x64