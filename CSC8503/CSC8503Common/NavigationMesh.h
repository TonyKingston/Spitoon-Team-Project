#pragma once
#include "NavigationMap.h"
#include "../../Common/Plane.h"
#include <string>
#include <vector>
namespace NCL {
	namespace CSC8503 {
		struct NavTri {
			Plane   triPlane;
			NCLVector3 centroid;
			float	area;
			NavTri* neighbours[3];
			NavTri* parent;

			//PathFinding
			float f;
			float g;

			int indices[3];

			NavTri() {
				f = 0;
				g = 0;
				area = 0.0f;
				parent = nullptr;
				neighbours[0] = nullptr;
				neighbours[1] = nullptr;
				neighbours[2] = nullptr;

				indices[0] = -1;
				indices[1] = -1;
				indices[2] = -1;
			}
			~NavTri() {}
		};

		class NavigationMesh : public NavigationMap	{
		public:
			NavigationMesh();
			NavigationMesh(const std::string&filename);
			~NavigationMesh();

			bool FindPath(const NCLVector3& from, const NCLVector3& to, NavigationPath& outPath) override;
			bool isReached(NCLVector3 position, NCLVector3 wayPoint);
		
			NavTri* GetTriForPosition(const NCLVector3& pos);

			std::vector<NavTri>		allTris;
			std::vector<NCLVector3>	allVerts;

			float maxX = 0, minX = 0, maxZ = 0, minZ = 0, minY = 0, maxY = 0;
			float mapSize = 0;
		private:
			NavTri* RemoveBestNode(std::vector<NavTri*>& list) const;
			bool  NodeInList(NavTri* n, std::vector<NavTri*>& list) const;
			float Heuristic(NavTri* hNode, NavTri* endNode) const;
		};
	}
}