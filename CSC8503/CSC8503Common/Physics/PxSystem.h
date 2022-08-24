#pragma once
#include "../SystemDefines.h"
#include "../../Common/NCLVector3.h"
#include "../../Common/Quaternion.h"
#include "../Transform.h"
#include "../GameObject.h"
#include "RigidBody.h"
#include "BasicErrorCallback.h"
#include "../GameWorld.h"
#include "../Ray.h"
#include "Types.h"
#include <PxPhysicsAPI.h>
#include "common/windows/PxWindowsDelayLoadHook.h"
#include <vector>
#include <map>

using namespace physx;
namespace NCL {
	namespace CSC8503 {

		struct PxLoad : public PxDelayLoadHook {

			virtual const char* getPhysXDllName() const {
				return "dll\\PhysX_x64.dll";
			}

			virtual const char* getPhysXCommonDllName() const {
				return "dll\\PhysXCommon_x64.dll";
			}

			virtual const char* getPhysXFoundationDllName() const {
				return "dll\\PhysXFoundation_x64.dll";
			}

			virtual const char* getPhysXCookingDllName() const {
				return "dll\\PhysXCooking_x64.dll";
			}

		};

		class PxSystem : public PxSimulationEventCallback, public PxBroadPhaseCallback {
		public:
			PxSystem(GameWorld& g);
			~PxSystem();

			void SetGravity(NCLVector3 g);
			void AddRigidBody(RigidBody* body);
			void AddRigidBodies(vector<RigidBody*> bodies);
			void RemoveRigidBody(RigidBody* body);
			void Update(float dt);
			void Clear();

			bool Raycast(Ray& ray, RayCollision& closestCollision, CollisionLayers layers = CollisionLayers::Default);
		
			PxPhysics* GetPhysics() {
				return pxPhysics;
			}

			PxCooking* GetCooking() {
				return pxCooking;
			}

			int GetNumberOfActors() {
				return pxScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, buffer, 10);
			}

			bool CreateAggregate(vector<RigidBody*> bodies, bool selfCollisions);
			void RemoveAggregate(PxAggregate* aggregate, bool andRelease);

			void SerializeScene(bool toBinary);
			void DeserializeScene();

			void ToggleDebugMode(bool mode);
			void RenderDebugInfo();

			PxMaterial* GetMaterial(string name) {
				PxMaterial* material = nullptr;
				if (pxMaterials.find(name) != pxMaterials.end()) {
					material = pxMaterials[name];
				}
			}

		private:

			void UpdateTransforms();

			// Implements PxSimulationEventCallback
			virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
			virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
			virtual void onConstraintBreak(PxConstraintInfo*, PxU32) {}
			virtual void onWake(PxActor**, PxU32) {}
			virtual void onSleep(PxActor**, PxU32) {}
			virtual void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}

			// Implements PxBroadPhaseCallback
			virtual void onObjectOutOfBounds(PxShape& shape, PxActor& actor) override;
			virtual void onObjectOutOfBounds(PxAggregate& aggregate) override {}

			const float GRAVITY = -9.81;
			PxDefaultAllocator		pxAllocator;
			BasicErrorCallback	pxErrorCallback;
			PxDefaultCpuDispatcher* pxDispatcher = NULL;
			PxTolerancesScale pxToleranceScale;

			PxFoundation* pxFoundation = NULL;
			PxPhysics* pxPhysics = NULL;
			PxCooking* pxCooking = NULL;

			PxScene* pxScene = NULL;
			PxActor** buffer = new PxActor*[10];
			void* scratchBlock;

			PxPvd* pxPvd = NULL;

			float dTOffset;

			GameWorld& gameWorld;

			bool inDebug = false;
		
			std::map<string, PxMaterial*> pxMaterials;

		};

	}
}
