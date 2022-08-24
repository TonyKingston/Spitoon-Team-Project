#include "GamePushdownState_UI.h"
#ifdef _WIN64
typedef PushdownState::PushdownResult ReturnResult;
auto SetDebug = [](CourseworkGame* game, bool change, bool isNetworked) {
	game->GetPhysics()->ToggleDebugMode(game->GetDebugMode());
	game->SetChangeColor(change);
	if(!isNetworked)
		game->GetRenderer()->inSplitScreen = !change;
};

#pragma region End State
ReturnResult EndState::OnUpdate(float dt, PushdownState** newState) {
	if (end_menu->AgainGame) {
		*newState = new StartState(game);
		return PushdownResult::Push;
	}
	else if (end_menu->QuitGame) {
		exit(0);
	}

	game->UpdateStart();
	return PushdownResult::NoChange;
}

void EndState::OnAwake() {
	game->GetUI()->AddMenu(end_menu);
	end_menu->WhoWinGame(game->GetRedScore(), game->GetBlueScore());
	audio::PauseBGM(true);
}

void EndState::OnSleep() {
	game->GetUI()->RemoveMenu(end_menu);
	audio::PauseBGM(false);
}
#pragma endregion

#pragma region Start State
ReturnResult StartState::OnUpdate(float dt, PushdownState** newState) {
	if (start_menu->EnterGame) {
		game->GetPhysics()->Clear();
		game->GetWorld()->ClearAndErase();
		game->LoadLevel(1);
		game->InitView();

		//Properties Related to Debug
		game->SetDebugMode(false);
		game->GetPhysics()->ToggleDebugMode(game->GetDebugMode());
		game->SetChangeColor(false);
		game->GetRenderer()->inSplitScreen = true;
		game->isNetworked = false;
		*newState = new GamingState(game);
		return PushdownResult::Push;
	}
	else if (start_menu->NetworkGame){
		//game->onUserCreate();
		game->GetPhysics()->Clear();
		game->GetWorld()->ClearAndErase();
		game->LoadLevel(2);

		//Properties Related to Debug
		game->SetDebugMode(false);
		game->GetPhysics()->ToggleDebugMode(game->GetDebugMode());
		game->SetChangeColor(false);
		game->GetRenderer()->inSplitScreen = false;
		game->isNetworked = true;
		*newState = new GamingState(game);
		return PushdownResult::Push;
	}

	/* Rotating logo animation */



	//else if (start_menu->DebugMode) {
	//	game->GetPhysics()->Clear();
	//	game->GetWorld()->ClearAndErase();
	//	game->LoadLevel(1);
	//	game->InitView();

	//	//Properties Related to Debug
		//game->SetDebugMode(true);
		//game->GetPhysics()->ToggleDebugMode(game->GetDebugMode());
		//game->SetChangeColor(true);
		//game->GetRenderer()->inSplitScreen = false;
	//

	//	*newState = new GamingState(game);
	//	return PushdownResult::Push;
	//}
	
	else if (start_menu->QuitGame) {
		exit(0);
	}
	game->UpdateStart();
	return PushdownResult::NoChange;
}
#include <filesystem>
#include "../../Common/TextureLoader.h"

void StartState::OnAwake() {
	game->GetUI()->AddMenu(start_menu);
	audio::PauseBGM(true);

	// Loading in Logo for loading animation
	std::filesystem::path path = "../../Assets/Textures/spitton.png";
	if (!std::filesystem::exists(path)) {
		return;
	}
	TextureBase* newTex = NCL::TextureLoader::LoadAPITexture("spitton.png");
	logoTex = (OGLTexture*) newTex;
}

void StartState::OnSleep() {
	game->GetUI()->RemoveMenu(start_menu);
	audio::PauseBGM(false);
}
#pragma endregion

#pragma region Gaming State
void GamingState::OnAwake() {
	game->GetUI()->dFont->Scale = 1.5f;
	game->GetUI()->AddMenu(debug_menu);
	game->GetUI()->AddMenu(time_menu);
	game->GetUI()->AddMenu(score_menu);
}

void GamingState::OnSleep() {
	game->GetUI()->RemoveMenu(debug_menu);
	game->GetUI()->RemoveMenu(time_menu);
	game->GetUI()->RemoveMenu(score_menu);
	game->GetUI()->dFont->Scale = 1.0f;
}

ReturnResult GamingState::OnUpdate(float dt, PushdownState** newState) {
	if (!game->isNetworked)
	{
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			*newState = new PauseState(game);
			return PushdownResult::Push;
		}
	}

	else if (time_menu->amount_time <= 0.0f) {
		*newState = new EndState(game);
		return PushdownResult::Push;
	}

	SetDebug(game, game->GetDebugMode(), game->isNetworked);

	game->UpdateGame(dt);
	bool go = game->startReady();
	if (go)
	{
		if (!game->GetDebugMode()) {
			time_menu->AddTime(dt);
		}
		if (game->GetCleaner()) {
			time_menu->GetTime_AI(game->GetCleaner()->GetCounter());
		}
		if (game->GetCleaner02()) {
			time_menu->GetTime_AI02(game->GetCleaner02()->GetCounter());
		}
		score_menu->GetScore(game->GetRedScore(), game->GetBlueScore());
	}
	return PushdownResult::NoChange;
}
#pragma endregion

#pragma region Pause State
void PauseState::OnAwake() {
	game->GetUI()->AddMenu(pause_menu);
	audio::PauseBGM(true);
}

void PauseState::OnSleep() {
	game->GetUI()->RemoveMenu(pause_menu);
	audio::PauseBGM(false);
}

ReturnResult PauseState::OnUpdate(float dt, PushdownState** newState) {
	if (pause_menu->QuitGame) {
		exit(0);
	}
	else if (pause_menu->ResumeGame) {
		return PushdownResult::Pop;
	}

	game->UpdateStart();
	return PushdownResult::NoChange;
}

#pragma endregion
#endif _WIN64