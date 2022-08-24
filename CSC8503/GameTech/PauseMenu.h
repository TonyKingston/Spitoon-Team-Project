#pragma once

#include "../../UI/GUI.h"
#ifdef _WIN64

namespace NCL {
	namespace CSC8503 {
		class CourseworkGame;

		class PauseMenu :public Menu {
		public:
			PauseMenu(CourseworkGame* g) :game(g),QuitGame(false), ResumeGame(false), ShowDebug(false) { }
			~PauseMenu() = default;

			void Content() override;

			bool QuitGame;
			bool ResumeGame;
			bool ShowDebug;
		private:
			CourseworkGame* game;
			void ShowConfiguration();
		};
	}
}
#endif _WIN64
