#pragma once
#include "json.h"
#include "json-forwards.h"
#include <fstream>
#include <iostream>
#include <string>
#include "../CSC8503Common/GameObject.h"
#include <map>


namespace NCL {
	namespace CSC8503 {
		class SystemConfig {
		public:
			SystemConfig();
			~SystemConfig();

			void LoadConfig(string filename);
			int GetVariable(string parameter);
		private:
			std::map<string, int> variables;
		};
	}
}