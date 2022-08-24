#include "MeshAnimation.h"
#include "NCLMatrix4.h"
#include "Assets.h"

#include <fstream>
#include <string>

using namespace NCL;

MeshAnimation::MeshAnimation() {
	jointCount	= 0;
	frameCount	= 0;
	frameRate	= 0.0f;
}

MeshAnimation::MeshAnimation(const std::string& filename) : MeshAnimation() {
	std::ifstream file(Assets::ANIMDIR + filename);

	std::string filetype;
	int fileVersion;

	file >> filetype;

	if (filetype != "MeshAnim") {
		std::cout << "File is not a MeshAnim file!" << std::endl;
		return;
	}
	file >> fileVersion;
	file >> frameCount;
	file >> jointCount;
	file >> frameRate;

	allJoints.reserve((size_t)frameCount * jointCount);

	for (unsigned int f = 0; f < frameCount; ++f) {
		for (unsigned int j = 0; j < jointCount; ++j) {
			NCLMatrix4 mat;
			for (int i = 0; i < 16; ++i) {
				file >> mat.array[i];
			}
			allJoints.emplace_back(mat);
		}
	}
}

MeshAnimation::~MeshAnimation() {

}

const NCLMatrix4* MeshAnimation::GetJointData(unsigned int frame) const {
	if (frame >= frameCount) {
		return nullptr;
	}
	int matStart = frame * jointCount;

	NCLMatrix4* dataStart = (NCLMatrix4*)allJoints.data();

	return dataStart + matStart;
}