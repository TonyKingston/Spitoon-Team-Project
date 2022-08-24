#pragma once
#include "GUI.h"
#ifdef _WIN64

namespace NCL {
	namespace CSC8503 {
		class TimeMenu :public Menu {
		public:
			TimeMenu() :amount_time(-1.0f), AI_time(-1.0),AI_time02(-1.0) {  }
			~TimeMenu() = default;

			void Content() override;
			void AddTime(const float &dt);

			void GetTime_AI(const float& time) { AI_time = time; }
			void GetTime_AI02(const float& time) { AI_time02 = time; }

			float amount_time;
		private:
			float AI_time;
			float AI_time02;
		};
	}
}
#endif _WIN64
