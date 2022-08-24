#pragma once

#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "CourseworkGame.h"

#include "../../UI/StartMenu.h"
#include "DebugMenu.h"
#include "../../UI/TimeMenu.h"
#include "../../UI/ScoreMenu.h"
#include "../../UI/GameEndMenu.h"
#include "PauseMenu.h"
#include "Audio.h"
#ifdef _WIN64

class EndState : public PushdownState {
public:
	EndState(CourseworkGame* tg) : game(tg) { 
		end_menu.reset(new GameEndMenu()); 
	}
	~EndState() {}
	PushdownResult OnUpdate(float dt, PushdownState** newState) override;

	void OnAwake() override;
	void OnSleep() override;
private:
	CourseworkGame* game;

	std::shared_ptr<GameEndMenu> end_menu;
};


class PauseState : public PushdownState {
public:
	PauseState(CourseworkGame* tg) : game(tg) { pause_menu.reset(new PauseMenu(game)); }
	~PauseState() {}
	PushdownResult OnUpdate(float dt, PushdownState** newState) override;

	void OnAwake() override;
	void OnSleep() override;

private:
	CourseworkGame* game;

	std::shared_ptr<PauseMenu> pause_menu;
};

class GamingState : public PushdownState {
public:
	GamingState(CourseworkGame* tg): game(tg) { 
		debug_menu.reset(new DebugMenu(game));
		time_menu.reset(new TimeMenu());
		score_menu.reset(new ScoreMenu());
		time_menu->amount_time = 120.0f;
	}
	~GamingState() {}
	PushdownResult OnUpdate(float dt, PushdownState** newState) override;

	void OnAwake() override;
	void OnSleep() override;

private:
	CourseworkGame* game;

	std::shared_ptr<DebugMenu> debug_menu;
	std::shared_ptr<TimeMenu> time_menu;
	std::shared_ptr<ScoreMenu> score_menu;
};

class StartState : public PushdownState {
public:
	StartState(CourseworkGame* tg): game(tg) { 
		start_menu.reset(new StartMenu()); 
	}
	~StartState() {}
	PushdownResult OnUpdate(float dt, PushdownState** newState) override;

	void OnAwake() override;
	void OnSleep() override;

private:
	CourseworkGame* game;

	std::shared_ptr<StartMenu> start_menu;
	OGLTexture* logoTex;
};
#endif _WIN64
