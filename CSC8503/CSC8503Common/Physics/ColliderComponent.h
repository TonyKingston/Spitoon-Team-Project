#pragma once
#include <PxPhysicsAPI.h>

using namespace physx;

namespace NCL {
	namespace CSC8503 {
		class ColliderComponent {
		public:

			ColliderComponent(PxGeometry& geometry, PxMaterial& material);
			~ColliderComponent();
			PxShape* GetShape() { return shape; }
			void SetShape(PxShape* shape) { this->shape = shape; }

			bool IsTrigger() { return isTrigger; }
			void SetTrigger(bool isTrigger);
		private:
			PxShape* shape;
			PxMaterial* material;
			bool isTrigger;
		};
	}
}
