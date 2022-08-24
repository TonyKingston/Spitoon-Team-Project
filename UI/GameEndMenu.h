#pragma once
#ifdef _WIN64
#include "GUI.h"

enum WinStates { RedWin, BlueWin, Draw };

namespace NCL {
	namespace CSC8503 {
		class GameEndMenu :public Menu {
		public:
			GameEndMenu() :AgainGame(false), QuitGame(false) {  }
			~GameEndMenu() = default;

			void Content() override;
			void WhoWinGame(const int& RedScore, const int& BlueScore)
			{
				red_score = RedScore;
				blue_score = BlueScore;
				if (RedScore > BlueScore)
					win_state = RedWin;
				else if (BlueScore > RedScore)
					win_state = BlueWin;
				else
					win_state = Draw;
			}

			bool AgainGame;
			bool QuitGame;

			int red_score;
			int blue_score;
			WinStates win_state;
		};
	}
}
#endif