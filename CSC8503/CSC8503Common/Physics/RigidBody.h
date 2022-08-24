#pragma once
#include "../../Common/NCLVector3.h"
#include "../../Common/NCLMatrix3.h"
#include "Types.h"
#include <PxPhysicsAPI.h>
#include "../Ray.h"
#include "ColliderComponent.h"
#include <vector>

using namespace NCL::Maths;
using namespace NCL;
using namespace physx;

namespace NCL {
	class MeshGeometry;

	namespace CSC8503 {

		class GameObject;
		class Transform;
		class PxSystem;

		struct Filter {
			int group;
			int mask;
		};

		class RigidBody {
		public:
			RigidBody(Transform* parentTransform, bool isStatic = false, float sFriction = 0.5, float dFriction = 0.5, float restitution = 0.5);
			RigidBody(Transform* parentTransform, RigidBodyParams params) : RigidBody(parentTransform, params.isStatic, params.sFriction, params.sFriction, params.restitution) {}
			~RigidBody();

			static void InitPhysics(PxSystem* physics);

			GameObject* GetGameObject() const {
				return gameObject;
			}

			NCLVector3 GetLinearVelocity() const {
				PxVec3 vec = dynamic ? dynamic->getLinearVelocity() : PxVec3();
				return NCLVector3(vec.x, vec.y, vec.z);
			}

			NCLVector3 GetAngularVelocity() const {
				PxVec3 vec = dynamic ? dynamic->getAngularVelocity() : PxVec3();
				return NCLVector3(vec.x, vec.y, vec.z);

			}

			void SetInverseMass(float invMass) const {
				if (dynamic) {
					PxRigidBodyExt::setMassAndUpdateInertia(*(PxRigidBody*)body, 1 / invMass);
				}
			}

			void SetGravityDisable(bool isGravity) const {
				body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, isGravity);
			}

			float GetInverseMass() const {
				return dynamic ? dynamic->getInvMass() : 0.0f;
			}

			void ApplyAngularImpulse(const NCLVector3& force);
			void ApplyLinearImpulse(const NCLVector3& force);

			float GetLinearDamping() const {
				return dynamic ? dynamic->getLinearDamping() : 0.0f;
			}

			void SetLinearDamping(float damping) {
				if (dynamic) {
					dynamic->setLinearDamping(damping);
				}
			}

			float GetAngularDamping() const {
				return dynamic ? dynamic->getAngularDamping() : 0.0f;
			}

			void SetAngularDamping(float damping) {
				if (dynamic) {
					dynamic->setAngularDamping(damping);
				}
			}

			void AddForce(const NCLVector3& force);
			void AddForceAtPosition(const NCLVector3& force, const NCLVector3& position);

			void AddTorque(const NCLVector3& torque);

			void ClearForces();

			void SetGlobalPosition(Transform* transform);

			void SetLinearVelocity(const NCLVector3& v) {
				if (dynamic) {
					dynamic->setLinearVelocity(v.ToPxVec3());
				}
			}

			void SetAngularVelocity(const NCLVector3& v) {
				if (dynamic) {
					dynamic->setAngularVelocity(v.ToPxVec3());
				}
			}

			void AddBoxShape(const NCLVector3& halfExtents);
			void AddSphereShape(float radius);
			void AddCapsuleShape(float radius, float halfHeight, bool upright = true);
			void AddPlaneShape();
			void AddConvexMesh(MeshGeometry* mesh);

			physx::PxRigidActor* GetPxBody();
			physx::PxMaterial* GetPxMaterial();

			float GetRestitution() const {
				return material->getRestitution();
			}

			void SetRestitution(float r) {
				material->setRestitution(r);
			}

			float GetStaticFriction() const {
				return material->getStaticFriction();
			}

			void SetStaticFriction(float f) {
				material->setStaticFriction(f);
			}

			float GetDynamicFriction() {
				return material->getDynamicFriction();
			}

			void SetDynamicFriction(float f) {
				material->setDynamicFriction(f);
			}

			bool IsAsleep() {
				return dynamic ? dynamic->isSleeping() : false;
			}


			void SetSleepThreshold(float threshold) {
				if (dynamic) {
					dynamic->setSleepThreshold(threshold);
				}
			}
			
			void SetWakeCounter(float threshold) {
				if (dynamic) {
					dynamic->setWakeCounter(threshold);
				}
			}

			void PutToSleep() {
				if (dynamic && !dynamic->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC)) {
					dynamic->putToSleep();
				}
			}

			void WakeUp() {
				if (dynamic && !dynamic->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC)) {
					dynamic->wakeUp();
				}
			}

			bool IsStatic() {
				return dynamic ? false : true;
			}

			bool IsTrigger() {
				return isTrigger;
			}

			bool IsKiematic() {
				return isKinematic;
			}

			void SetTrigger(bool val) {
				isTrigger = val;
				for (auto shape : shapes) {
					shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
					shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
				}
			}

			void SetKinematic(bool kinematic) {
				if (dynamic) {
					dynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);
					isKinematic = kinematic;
				}
			}

			void SetKinematicTarget(Transform* target);

			void Lock(int constraints) {
				if (dynamic) {
					dynamic->setRigidDynamicLockFlags(static_cast<PxRigidDynamicLockFlag::Enum>(constraints));
				}
			}

			// Make sure to call this after the shapes have been added to the rigid body
			void SetCollisionFilters(const CollisionLayers& filterGroup, const CollisionLayers& filterMask);

			void SetIsCollidable(bool val) {
				for (auto shape : shapes) {
					shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, val);
				}
			}

			void SetIsRaycastable(bool val) {
				for (auto shape : shapes) {
					shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, val);
				}
			}

			void SetSolverIterationCount(int positionIters = 4, int velocityIters = 1) {
				if (dynamic) {
					dynamic->setSolverIterationCounts(positionIters, velocityIters);
				}
			}

			bool Raycast(Ray& ray, RayCollision& closestCollision, RigidBody* target);
			
			void VisualizeDebugVolume(bool val) {
				for (auto shape : shapes) {
					shape->setFlag(PxShapeFlag::eVISUALIZATION, val);
				}
			}


		private:

			inline PxTransform GetGlobalPose(const PxShape& shape);
			void AddShape(PxShape* shape, bool debugVisualize = true);

			Transform* transform;

			bool isKinematic = false;
			bool isTrigger = false;

			std::vector<PxShape*> shapes;
			Filter filter;
			PxRigidDynamic* dynamic;
			PxRigidActor* body;
			PxMaterial* material;

			static physx::PxPhysics* pxp;
			static physx::PxCooking* cooking;
			GameObject* gameObject;

			const float REST_OFFSET = 0.01f;
			const float CONTACT_OFFSET = 0.02f;

			friend class GameObject;
		};
	}
}

