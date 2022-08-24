#include "NavigationMesh.h"
#include "../../Common/Assets.h"
#include "../../Common/Maths.h"
#include <fstream>
using namespace NCL;
using namespace CSC8503;
using namespace std;

NavigationMesh::NavigationMesh()
{
}

NavigationMesh::NavigationMesh(const std::string&filename)
{
	ifstream file(Assets::DATADIR + filename);

	int numVertices = 0;
	int numIndices	= 0;

	file >> numVertices;
	file >> numIndices;

	for (int i = 0; i < numVertices; ++i) {
		NCLVector3 vert;
		file >> vert.x;
		file >> vert.y;
		file >> vert.z;

		allVerts.emplace_back(vert);
	}

	allTris.resize(numIndices / 3);

	for (int i = 0; i < allTris.size(); ++i) {
		NavTri* tri = &allTris[i];
		file >> tri->indices[0];
		file >> tri->indices[1];
		file >> tri->indices[2];

		tri->centroid = allVerts[tri->indices[0]] +
			allVerts[tri->indices[1]] +
			allVerts[tri->indices[2]];

		tri->centroid = allTris[i].centroid / 3.0f;

		tri->triPlane = Plane::PlaneFromTri(allVerts[tri->indices[0]],
			allVerts[tri->indices[1]],
			allVerts[tri->indices[2]]);

		tri->area = Maths::CrossAreaOfTri(allVerts[tri->indices[0]], allVerts[tri->indices[1]], allVerts[tri->indices[2]]);
	}
	for (int i = 0; i < allTris.size(); ++i) {
		NavTri* tri = &allTris[i];
		for (int j = 0; j < 3; ++j) {
			int index = 0;
			file >> index;
			if (index != -1) {
				tri->neighbours[j] = &allTris[index];
			}
		}
		mapSize += tri->area;
	}
	
	//find the maxX and maxZ
	maxX = allVerts[0].x;
	maxZ = allVerts[0].z;
	minX = allVerts[0].x;
	minZ = allVerts[0].z;
	for (auto i = allVerts.begin() + 1; i != allVerts.end(); i++) {
		if ((*i).x > maxX) {
			maxX = (*i).x;
		}
		if ((*i).z > maxZ) {
			maxZ = (*i).z;
		}
		if ((*i).y > maxY) {
			maxY = (*i).y;
		}
		if ((*i).x < minX) {
			minX = (*i).x;
		}
		if ((*i).z < minZ) {
			minZ = (*i).z;
		}
		if ((*i).y < minY) {
			minY = (*i).y;
		}
	}
}

NavigationMesh::~NavigationMesh()
{
}

bool NavigationMesh::FindPath(const NCLVector3& from, const NCLVector3& to, NavigationPath& outPath) {
	if (from.x < minX || from.z < minZ || from.x > maxX || from.z > maxZ || from.y < minY || from.y > maxY ||
		to.x < minX || to.z < minZ || to.x > maxX || to.z > maxZ || to.y < minY || to.y > maxY) {
		std::cout << "out of range!" << std::endl;
		return false;
	}

	NavTri* start	= GetTriForPosition(from);
	NavTri* end	= GetTriForPosition(to);

	if (start == nullptr || end == nullptr) {
		return false;
	}

	if (start == end){
		outPath.PushWaypoint(from);
		return true;
	}

	std::vector<NavTri*>  openList;
	std::vector<NavTri*>  closedList;

	openList.emplace_back(start);

	start->f = 0;
	start->g = 0;
	start->parent = nullptr;

	NavTri* currentBestNode = nullptr;

	while (!openList.empty()) {
		currentBestNode = RemoveBestNode(openList);

		if (currentBestNode == end) {			//we've found the path!
			NavTri* node = end;
			while (node != nullptr) {
				//outPath.PushWaypoint(node->position);
				outPath.PushWaypoint(node->centroid);
				node = node->parent;
			}
			return true;
		}
		else {
			for (int i = 0; i < 3; ++i) {
				NavTri* neighbour = currentBestNode->neighbours[i];
				if (!neighbour) { //might not be connected...
					continue;
				}
				bool inClosed = NodeInList(neighbour, closedList);
				if (inClosed) {
					continue; //already discarded this neighbour...
				}

				float h = Heuristic(neighbour, end);
				float g = (neighbour->centroid - currentBestNode->centroid).Length();
				float f = h + g;

				bool inOpen = NodeInList(neighbour, openList);

				if (!inOpen) { //first time we've seen this neighbour
					openList.emplace_back(neighbour);
				}
				if (!inOpen || f < neighbour->f) {//might be a better route to this neighbour
					neighbour->parent = currentBestNode;
					neighbour->f = f;
					neighbour->g = g;
				}
			}
			closedList.emplace_back(currentBestNode);
		}
	}

	std::cout << "no" << std::endl;
	return false;
}

/*
If you have triangles on top of triangles in a full 3D environment, you'll need to change this slightly,
as it is currently ignoring height. You might find tri/plane raycasting is handy.
*/

NavTri* NavigationMesh::GetTriForPosition(const NCLVector3& pos) {
	for (NavTri& t : allTris) {
		NCLVector3 planePoint = t.triPlane.ProjectPointOntoPlane(pos);

		float ta = Maths::CrossAreaOfTri(allVerts[t.indices[0]], allVerts[t.indices[1]], planePoint);
		float tb = Maths::CrossAreaOfTri(allVerts[t.indices[1]], allVerts[t.indices[2]], planePoint);
		float tc = Maths::CrossAreaOfTri(allVerts[t.indices[2]], allVerts[t.indices[0]], planePoint);

		float areaSum = ta + tb + tc;

		if (abs(areaSum - t.area)  > 0.001f) { //floating points are annoying! Are we more or less inside the triangle?
			continue;
		}
		return &t;
	}
	return nullptr;
}

float NavigationMesh::Heuristic(NavTri* hNode, NavTri* endNode) const {
	return (hNode->centroid - endNode->centroid).Length();
}

NavTri* NavigationMesh::RemoveBestNode(std::vector<NavTri*>& list) const {
	std::vector<NavTri*>::iterator bestI = list.begin();

	NavTri* bestNode = *list.begin();

	for (auto i = list.begin(); i != list.end(); ++i) {
		if ((*i)->f < bestNode->f) {
			bestNode = (*i);
			bestI = i;
		}
	}
	list.erase(bestI);

	return bestNode;
}

bool NavigationMesh::NodeInList(NavTri* n, std::vector<NavTri*>& list) const {
	std::vector<NavTri*>::iterator i = std::find(list.begin(), list.end(), n);
	return i == list.end() ? false : true;
}

bool NavigationMesh::isReached(NCLVector3 position, NCLVector3 wayPoint) {
	float distance = (position - wayPoint).Length();
	if (distance <= 4.0f) {
		return true;
	}
	return false;
}