#pragma once

#include "GUI.h"
#ifdef _WIN64
namespace NCL {
	namespace CSC8503 {

		class ScoreMenu :public Menu {
		public:
			ScoreMenu() : red_score(-1), blue_score(-1) { }
			~ScoreMenu() = default;

			void Content() override;

			void GetScore(const int& RedScore, const int& BlueScore) { red_score = RedScore; blue_score = BlueScore; }
		private:
			int red_score;
			int blue_score;
		};
	}
}
#endif _WIN64