#pragma once
#include "../../Common/NCLVector3.h"
#include "../../Common/Quaternion.h"

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		class GameObject;
		class NetworkState {
		public:
			NetworkState();
			virtual ~NetworkState();

			NCLVector3		position;
			Quaternion	orientation;
			int			stateID;
		};
	}
}

