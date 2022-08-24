#pragma once

#include "GUI.h"
#ifdef _WIN64
namespace NCL {
	namespace CSC8503 {
		class CourseworkGame;

		class StartMenu :public Menu {
		public:
			StartMenu() :
				 EnterGame(false), QuitGame(false), NetworkGame(false) { }
			~StartMenu() = default;

			void Content() override;

			bool EnterGame;
			bool NetworkGame;
			bool QuitGame;
		};
	}
}
#endif _WIN64
