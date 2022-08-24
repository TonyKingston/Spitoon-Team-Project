#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/PushdownState.h"
#include "CourseworkGame.h"

namespace NCL {

	namespace CSC8503 {
#ifdef _WIN64
		class MainMenuState : public PushdownState {
		public:
			MainMenuState(CourseworkGame* tg, RendererBase* r);
			~MainMenuState() {}
			PushdownResult OnUpdate(float dt, PushdownState** newState) override;

			void OnAwake() override;

		private:
			OGLRenderer* renderer;
			CourseworkGame* game;
			int currentChoice;
			int minChoice;
			int maxChoice;
		};

#endif _WIN64

	}
}