#pragma once

#include "../../Common/Camera.h"
#include "../../Common/Plane.h"

#include "../Transform.h"
#include "../GameObject.h"

#include "../Ray.h"
#include "Types.h"

using NCL::Camera;
using namespace NCL::Maths;
using namespace NCL::CSC8503;
namespace NCL {
	class CollisionDetection
	{
	public:
		
		struct CollisionInfo {
			GameObject* a;
			GameObject* b;
			mutable int		framesLeft;

			ContactPoint point;

			void AddContactPoint(const NCLVector3& localA, const NCLVector3& localB, const NCLVector3& normal, float p) {
				point.localA = localA;
				point.localB = localB;
				point.normal = normal;
				point.penetration = p;
			}

			//Advanced collision detection / resolution
			bool operator < (const CollisionInfo& other) const {
				size_t otherHash = (size_t)other.a->GetWorldID() + ((size_t)other.b->GetWorldID() << 32);
				size_t thisHash = (size_t)a->GetWorldID() + ((size_t)b->GetWorldID() << 32);

				return (thisHash < otherHash);
			}

			bool operator ==(const CollisionInfo& other) const {
				if (other.a == a && other.b == b) {
					return true;
				}
				return false;
			}
		};


		static Ray BuildRayFromMouse(const Camera& c);

		static NCLVector3 Unproject(const NCLVector3& screenPos, const Camera& cam);

		static NCLVector3		UnprojectScreenPosition(NCLVector3 position, float aspect, float fov, const Camera& c);
		static NCLMatrix4		GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane);
		static NCLMatrix4		GenerateInverseView(const Camera& c);

	private:
		CollisionDetection() {}
		~CollisionDetection() {}
	};
}

