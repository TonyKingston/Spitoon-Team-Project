#pragma once
#include "../../Common/NCLVector3.h"
#include "../../Common/Plane.h"

namespace NCL {
	namespace Maths {
		struct RayCollision {
			void*		node;			//Node that was hit
			NCLVector3		collidedAt;		//WORLD SPACE position of the collision!
			float		rayDistance;
			NCLVector3     collisionNormal;

			RayCollision() {
				node			= nullptr;
				rayDistance		= FLT_MAX;
			}
		};

		class Ray {
		public:
			Ray(NCLVector3 position, NCLVector3 direction) {
				this->position  = position;
				this->direction = direction;
			}
			~Ray(void) {}

			NCLVector3 GetPosition() const {return position;	}

			NCLVector3 GetDirection() const {return direction;	}

		protected:
			NCLVector3 position;	//World space position
			NCLVector3 direction;	//Normalised world space direction

		};
	}
}