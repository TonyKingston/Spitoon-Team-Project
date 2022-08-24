#pragma once
//Navigation
#include "..\CSC8503Common\GameObject.h"
#include "../CSC8503Common/NavigationMesh.h"
#include "../CSC8503Common/NavigationPath.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"
//Others
#include "../CSC8503Common/Debug.h"
#include <vector>
#include <algorithm>
#include "../../Common/NCLVector3.h"

namespace NCL {
	namespace CSC8503 {

		using NCL::Maths::NCLVector3;
		class Cleaner :public GameObject {
		public:
			Cleaner(NavigationMesh* navMesh, vector<GameObject*>* testPaint);
			~Cleaner();

			virtual void Update(float dt);

			const int &GetCounter() { return counter; }
		private:
			float counter;
			/*Navigation*/
			std::vector<GameObject*>* _testPaint;
			std::vector<GameObject*> myPaint;
			std::vector<NCLVector3> myPathfindingNodes;
			NavigationMesh* myMesh;
			NavigationPath myPath;
			NavigationPath testPath;
			NCLVector3 startPos;
			NCLVector3 endPos;
			NCLVector3 curwayPoint;
			void GeneratePath();
			void DisplayPath();

			/*Statemachine*/
			bool isCleaning;
			bool isFinished;
			StateMachine* machine;
			void InitStateMachine();

			/*Movement*/
			void DriveToPosition(const NCLVector3 position);
		};
	}
}