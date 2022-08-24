#pragma once
#include "NavigationMap.h"
#include "micropather.h"
#include <string>
#ifdef _x64
#include "../GameTech/GameTechRenderer.h" 


#endif
using namespace NCL;
using namespace micropather;
namespace NCL {
	namespace CSC8503 {
		enum GRIDTYPE {
			Blank,
			Block,
			Obstacle,
		};
		struct GridNode {
			GridNode* parent;

			GridNode* connected[4];
			int		  costs[4];

			NCLVector3		position;

			float f;
			float g;

			int type;
			GRIDTYPE gridType;

			GridNode() {
				for (int i = 0; i < 4; ++i) {
					connected[i] = nullptr;
					costs[i] = 0;
				}
				f = 0;
				g = 0;
				type = 0;
				parent = nullptr;
				gridType = GRIDTYPE::Blank;
			}
			~GridNode() {	}
		};

		class NavigationGrid : public NavigationMap{
		public:
			NavigationGrid();
			NavigationGrid(const std::string& filename);
			~NavigationGrid();


			int GetWidth() { return gridWidth; }
			int GetHeight() { return gridHeight; }
			GridNode* GetNodes() { return allNodes; }
			int nodeSize;

			bool FindPath(const NCLVector3& from, const NCLVector3& to, NavigationPath& outPath) override;
#ifdef _x64
			void DrawMap(GameTechRenderer* r);
			bool isReached(NCLVector3 position, NCLVector3 wayPoint);
#endif

		protected:
			bool		NodeInList(GridNode* n, std::vector<GridNode*>& list) const;
			GridNode* RemoveBestNode(std::vector<GridNode*>& list) const;
			float		Heuristic(GridNode* hNode, GridNode* endNode) const;
			int gridWidth;
			int gridHeight;
			int offset = 200;

			GridNode* allNodes;
		};
	}
}