#pragma once
#include <vector>
#include <string>

namespace NCL {

namespace Maths {
	class NCLMatrix4;
}
using namespace Maths;

class MeshAnimation
{
public:
	MeshAnimation();
	MeshAnimation(const std::string& filename);
	virtual ~MeshAnimation();

	unsigned int GetJointCount() const {
		return jointCount;
	}

	unsigned int GetFrameCount() const {
		return frameCount;
	}

	float GetFrameRate() const {
		return frameRate;
	}

	const NCLMatrix4* GetJointData(unsigned int frame) const;

protected:
	unsigned int	jointCount;
	unsigned int	frameCount;
	float			frameRate;

	std::vector<NCLMatrix4>		allJoints;
};
}

