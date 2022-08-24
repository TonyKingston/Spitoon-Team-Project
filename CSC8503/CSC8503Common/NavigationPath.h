#pragma once
#include "../../Common/NCLVector3.h"
#include <vector>

namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		class NavigationPath		{
		public:
			NavigationPath() {}
			~NavigationPath() {}

			void	Clear() {
				waypoints.clear();
			}
			int     GetPathSize() {
				return waypoints.size();
			}
			void	PushWaypoint(const NCLVector3& wp) {
				waypoints.emplace_back(wp);
			}
			bool	PopWaypoint(NCLVector3& waypoint) {
				if (waypoints.empty()) {
					return false;
				}
				waypoint = waypoints.back();
				waypoints.pop_back();
				return true;
			}

		protected:

			std::vector <NCLVector3> waypoints;
		};
	}
}

