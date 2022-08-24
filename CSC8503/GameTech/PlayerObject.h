#pragma once
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/Debug.h"
#include <vector>
#include <algorithm>

namespace NCL {
	namespace CSC8503 {
		class PlayerObject :public GameObject {
		public:
			PlayerObject() {
				this->SetOnCollisionBeingFunc([=](GameObject* otherActor, ContactPoint contact)->void {
					if (otherActor->GetName() == "Dash") {
						dashCollided = true;
						otherActor->SetToDelete(true);
					}
					if (otherActor->GetName() == "Speed") {
						speedCollided = true;
						otherActor->SetToDelete(true);
					}
					}
				);
			}
			~PlayerObject();

			bool speedCollided = false;
			bool dashCollided = false;


		};
	}
}