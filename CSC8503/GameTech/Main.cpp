
//JSON function test
//void ReadFile(const string& filename){
//	Json::Reader reader;
//	Json::Value root;
//	std::ifstream ifs("./JSON/" + filename);
//	if (!reader.parse(ifs, root))
//	{
//		std::cout << "fail to open file!" << std::endl;
//	}
//	else
//	{
//		std::cout << root["Name"].asString() << endl;
//	}
//}
//
//void WriteFile(const string& filename)
//{
//	Json::Value root;
//	Json::Value person;
//	Json::FastWriter writer;
//
//
//	person["age"] = 28;
//	person["name"] = "Min";
//	root["isMale"] = "Yes";
//	root["array"].append(person);
//
//
//	string json_file = writer.write(root);
//
//	ofstream ofs;
//	ofs.open("./JSON/" + filename);
//	assert(ofs.is_open());
//	ofs << json_file;
//}


/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/

#ifdef _WIN64

#include "CourseworkGame.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/NavigationMesh.h"
#include "../CSC8503Common/NavMeshRenderer.h"

//#include "NetworkedGame.h"
#include "../CSC8503Common/NetworkBase.h"

#include "../CSC8503Common/PushdownMachine.h"
#include "GamePushDownState_UI.h"

using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <string>
#include <yvals.h>
#include <json.h>
#include <value.h>
#include <cassert>
#include <fstream>


int FMOD_Main() {
	//
	Window* w = Window::CreateGameWindow("CSC8503 Game technology!", 1920, 1200);
	

	if (!w->HasInitialised()) {
		return -1;
	}	
	srand(time(0));
	w->ShowOSPointer(true);
	w->LockMouseToWindow(true);
	CourseworkGame* g = new CourseworkGame(w);
	PushdownMachine machine(new StartState(g));

	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!

	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		machine.Update(dt);
	}
	w->ShowConsole(false);
	w->DestroyGameWindow();
}
#endif _WIN64
