#pragma once
#include "../../UI/GUI.h"
#ifdef _WIN64

namespace NCL {
	namespace CSC8503 {
		class CourseworkGame;

		class DebugMenu :public Menu {
		public:
			DebugMenu(CourseworkGame* g) :game(g) { }
			~DebugMenu() = default;

			void Content() override;
			void ShowFPS();
			void ShowMemory();
		private:
			CourseworkGame* game;
		};
	}
}
#endif _WIN64