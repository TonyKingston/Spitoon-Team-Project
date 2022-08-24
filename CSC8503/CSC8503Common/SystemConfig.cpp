#include "SystemConfig.h"
#include "..//..//Common/Assets.h"

using namespace NCL::CSC8503;
using std::cout;
using std::endl;
using std::string;

SystemConfig::SystemConfig() {
}

SystemConfig::~SystemConfig() {
}

void SystemConfig::LoadConfig(string filename) {
	Json::CharReaderBuilder reader;
	Json::Value root;
	string path = Assets::JSONADIR + filename;
	reader["collectComments"] = false;
	string errs;
	std::ifstream ifs(path);
	if (!Json::parseFromStream(reader, ifs, &root, &errs)) {
		cout << "Failed to open config file!" << endl;
	}
	else {
		for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++) {
			variables.emplace(itr.name(), itr->asInt());
		}
	}
}

int SystemConfig::GetVariable(string parameter) {
	if (variables.find(parameter) != variables.end()) {
		return variables[parameter];
	}
	return false;
}
