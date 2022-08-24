#include "Cleaner.h"

using namespace std;
using namespace NCL;
using namespace CSC8503;

Cleaner::Cleaner(NavigationMesh* navMesh, vector<GameObject*>* testPaint) {
	counter = 15.0f;
	isCleaning = false;
	isFinished = false;
	myMesh = navMesh;
	_testPaint = testPaint;
	machine = new StateMachine();
	InitStateMachine();
}

Cleaner::~Cleaner() {
	delete myMesh;
	delete _testPaint;
	delete machine;
}

void Cleaner::InitStateMachine() {
	//waiting state
	State* stateA = new State([&](float dt)-> void
		{
	
			//transform.SetPosition(NCLVector3(transform.GetPosition().x, -4, transform.GetPosition().z));
			//rigidBody->SetGlobalPosition(&transform);
	
			/*std::cout << (*myPaintNum) << std::endl;*/
		}
	);

	//cleaning state
	State* stateB = new State([&](float dt)->void
		{
			if (myMesh->isReached(transform.GetPosition(), curwayPoint)) {
				//If cleaner has reached the current waypoint
				if (myPath.GetPathSize() == 0) {
					if (myPaint.size() != 0) {
						//If cleaner has reached the last waypoint of a NavigationPath and there're still destinations left
						GeneratePath();
						myPath.PopWaypoint(curwayPoint);
						DriveToPosition(curwayPoint);
					}
					else {
						//If cleaner has reached the last waypoint and there're no other destinations left
						isFinished = true;
					}
				}
				else {
					//If the cleaner has reached the current waypoint and there are still waypoints left
					myPath.PopWaypoint(curwayPoint);
					DriveToPosition(curwayPoint);
				}
			}
			else {
				//If the cleaner hasn't reached the current waypoint
				DriveToPosition(curwayPoint);
			}

		}
	);

	machine->AddState(stateA);
	machine->AddState(stateB);

	machine->AddTransition(new StateTransition(stateA, stateB,
		[&]()->bool
		{
			if (counter <= 0.0f)
			{
				//Save all paints' positions
				for (auto i = _testPaint->begin(); i != _testPaint->end(); i++) {
					myPaint.emplace_back((*i));
				}
				reverse(myPaint.begin(), myPaint.end());
				_testPaint->clear();

				//Initialise myPath
				GeneratePath();
				myPath.PopWaypoint(curwayPoint);
				
				//Enter the Cleaning State
				isCleaning = true;
				return true;
			}
			return false;
		}
	));
	machine->AddTransition(new StateTransition(stateB, stateA,
		[&]()->bool
		{
			if (isFinished) {
				counter = 15.0f;
				rigidBody->SetLinearVelocity(NCLVector3(0, 0, 0));
				rigidBody->SetAngularVelocity(NCLVector3(0, 0, 0));
				isCleaning = false;
				isFinished = false;
			
				myPath.Clear();
				myPaint.clear();
				
				
				return true;
			}
			return false;
		}
	));
}

void Cleaner::Update(float dt){
	//CoolDown
	if (!isCleaning && _testPaint->size() > 0) {
		counter -= dt;
	}
	machine->Update(dt);
}

void Cleaner::DisplayPath() {
	for (int i = 1; i < myPathfindingNodes.size(); ++i) {
		NCLVector3 a = myPathfindingNodes[i - 1];
		NCLVector3 b = myPathfindingNodes[i];

#ifdef _WIN64
		Debug::DrawLine(a, b, NCLVector4(1, 1, 0, 1));
#endif
	}
}

void Cleaner::GeneratePath() {
	//Initialise the startPos and endPos
	startPos = transform.GetPosition();
	endPos = myPaint.back()->GetTransform().GetPosition();
	myPaint.pop_back();

	//Generate Path
	myPath.PushWaypoint(endPos);
	myMesh->FindPath(startPos, endPos, myPath);

	//Test PathFinding
	//testPath.Clear();
	//myPathfindingNodes.clear();
	//testPath = myPath;
	//NCLVector3 waypointPos;
	//while (testPath.PopWaypoint(waypointPos)) {
	//	myPathfindingNodes.emplace_back(waypointPos);
	//}
}

void Cleaner::DriveToPosition(const NCLVector3 position) {
	NCLVector3 startTransform = transform.GetPosition() - NCLVector3(0, 1.5f, 0);
	NCLVector3 moveDir = (position - startTransform).Normalised();
	rigidBody->SetLinearVelocity(moveDir * (position - startTransform).Length() * 2);
}