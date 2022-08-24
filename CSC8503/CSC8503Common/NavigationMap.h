#pragma once
#include "../../Common/NCLVector3.h"
#include "NavigationPath.h"
namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		class NavigationMap
		{
		public:
			NavigationMap() {}
			~NavigationMap() {}

			virtual bool FindPath(const NCLVector3& from, const NCLVector3& to, NavigationPath& outPath) = 0;
		};
	}
}

