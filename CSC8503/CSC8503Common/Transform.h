#pragma once
#include "../../Common/NCLMatrix4.h"
#include "../../Common/NCLMatrix3.h"
#include "../../Common/NCLVector3.h"
#include "../../Common/Quaternion.h"
#include <vector>
#include "foundation/PxTransform.h"

using std::vector;

using namespace NCL::Maths;

namespace NCL {
	namespace CSC8503 {
		class Transform
		{
		public:
			Transform();
			~Transform();

			Transform& SetPosition(const NCLVector3& worldPos);
			Transform& SetScale(const NCLVector3& worldScale);
			Transform& SetOrientation(const Quaternion& newOr);

			NCLVector3 GetPosition() const {
				return position;
			}

			NCLVector3 GetScale() const {
				return scale;
			}

			Quaternion GetOrientation() const {
				return orientation;
			}

			NCLMatrix4 GetMatrix() const {
				return matrix;
			}
			void UpdateMatrix();

			NCLVector3 GetForwardFacing() const {
				return orientation * NCLVector3(0, 0, 1);
			}

			physx::PxTransform ToPxTransform() const {
				return physx::PxTransform(position.ToPxVec3(), orientation.ToPxQuat());
			}

			NCLMatrix4 Lerp(const NCLMatrix4& to, float p);

		protected:
			NCLMatrix4		matrix;
			Quaternion	orientation;
			NCLVector3		position;

			NCLVector3		scale;
		};
	}
}

