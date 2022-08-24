#pragma once
#include <foundation\PxVec3.h>
#include "../../Common/NCLVector3.h"
#include "../../Common/NCLVector4.h"
#include "../../Common/Quaternion.h"

using namespace physx;
using namespace NCL::Maths;

namespace NCL {
	class PxConversions {
	public:
		inline static PxVec3 ToPxVec3(NCLVector3 vec) {
			return PxVec3(vec.x, vec.y, vec.z);
		}

		inline static PxVec4 ToPxVec4(NCLVector4 vec) {
			return PxVec4(vec.x, vec.y, vec.z, vec.w);
		}

		inline static PxQuat ToPxQuat(Quaternion q) {
			return PxQuat(q.x, q.y, q.z, q.w);
		}

		inline static NCLVector3 PxToVec3(PxVec3 vec) {
			return NCLVector3(vec.x, vec.y, vec.z);
		}

		inline static NCLVector4 PxToVec4(PxVec4 vec) {
			return NCLVector4(vec.x, vec.y, vec.z, vec.w);
		}

		inline static Quaternion PxToQuat(PxQuat q) {
			return Quaternion(q.x, q.y, q.z, q.w);
		}
	};
}