#include "PxSystem.h"
#include "../../Common/GameTimer.h"
#include "PxConversions.h"
#include "extensions/PxCollectionExt.h"
#ifdef _WIN64
#include "../../CSC8503Common/Debug.h"
#endif _WIN64

using namespace NCL::CSC8503;

#define PX_SCRATCH_BLOCK_SIZE (1024 * 128)

PxFilterFlags PxFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {

	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)) {
		pairFlags = PxPairFlag::eNOTIFY_TOUCH_FOUND;
		return PxFilterFlag::eDEFAULT;
	}

	pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_CONTACT_POINTS;

	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)) {
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		return PxFilterFlag::eDEFAULT;
	}
	else {
		return PxFilterFlag::eKILL;
	}
}

PxSystem::PxSystem(GameWorld& g) : gameWorld(g) {
	pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, pxAllocator, pxErrorCallback);
	if (!pxFoundation) throw ("PxCreateFoundation failed!");

#ifdef _DEBUG
	pxPvd = PxCreatePvd(*pxFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif

	pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale(), true, pxPvd);
	if (!pxPhysics) throw ("PxCreatePhysics failed!");

	if (!PxInitExtensions(*pxPhysics, pxPvd)) throw ("PxCreatePhysics failed!");

	pxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *pxFoundation, PxCookingParams(PxTolerancesScale()));
	if (!pxCooking) throw ("PxCreateCooking failed!");

	PxSceneDesc sceneDesc = PxSceneDesc(pxPhysics->getTolerancesScale());

	//sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
	sceneDesc.gravity = PxVec3(0.0f, GRAVITY, 0.0f);
	pxDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = pxDispatcher;
	sceneDesc.filterShader = PxFilterShader;
	sceneDesc.simulationEventCallback = this;
	sceneDesc.broadPhaseCallback = this;
//	sceneDesc.limits = PxSceneLimits(40, 40, 20, 20, 0, 4, );
	pxScene = pxPhysics->createScene(sceneDesc);
	//PxBroadPhaseRegion broadRegion = PxBroadPhaseRegion();
	//broadRegion.bounds = (PxBounds3(PxVec3(-200, -200, -200), PxVec3(200, 200, 200)));
	//pxScene->addBroadPhaseRegion(broadRegion);

#ifdef  _DEBUG
	PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
#endif //  _DEBUG

	scratchBlock = new void*[PX_SCRATCH_BLOCK_SIZE];
}

PxSystem::~PxSystem() {
	SAFE_RELEASE(pxScene);
	SAFE_RELEASE(pxDispatcher);
	SAFE_RELEASE(pxPhysics);
	SAFE_RELEASE(pxCooking);
	if (pxPvd) {
		PxPvdTransport* transport = pxPvd->getTransport();
		SAFE_RELEASE(transport);
		SAFE_RELEASE(pxPvd);
	}
	SAFE_RELEASE(pxFoundation);
	delete[] buffer;
	delete[] scratchBlock;
}

void PxSystem::SetGravity(NCLVector3 g) {
	pxScene->setGravity(g.ToPxVec3());
}

void PxSystem::AddRigidBody(RigidBody* body) {
	pxScene->addActor(*body->GetPxBody());
}

void PxSystem::AddRigidBodies(vector<RigidBody*> bodies) {
	for (auto& body : bodies) {
		pxScene->addActor(*body->GetPxBody());
	}
}

void PxSystem::RemoveRigidBody(RigidBody* body) {
	pxScene->removeActor(*body->GetPxBody());
}

int constraintIterationCount = 10;

const int   idealHZ = 120;
const float idealDT = 1.0f / idealHZ;

int realHZ = idealHZ;
float realDT = idealDT;

void PxSystem::Update(float dt) {
	dTOffset += dt; //We accumulate time delta here - there might be remainders from previous frame!

	GameTimer t;
	t.GetTimeDeltaSeconds();

	while (dTOffset >= realDT) {
		pxScene->simulate(dt, NULL, scratchBlock, PX_SCRATCH_BLOCK_SIZE);
		pxScene->fetchResults(true);
		dTOffset -= realDT;
	}

	UpdateTransforms();
	t.Tick();
	float updateTime = t.GetTimeDeltaSeconds();

	if (updateTime > realDT) {
		realHZ /= 2;
		realDT *= 2;
		std::cout << "Dropping iteration count due to long physics time...(now " << realHZ << ")\n";
	}
	else if (dt * 2 < realDT) { //we have plenty of room to increase iteration count!
		int temp = realHZ;
		realHZ *= 2;
		realDT /= 2;

		if (realHZ > idealHZ) {
			realHZ = idealHZ;
			realDT = idealDT;
		}
		if (temp != realHZ) {
			std::cout << "Raising iteration count due to short physics time...(now " << realHZ << ")\n";
		}
	}
	
}

void PxSystem::Clear() {
	// The game world is responsible for deleting game objects, thus calling the destructor for the rigid bodies.
	int nbActors = pxScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	PxActor** buffer = new PxActor*[nbActors];
	pxScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, buffer, nbActors);
	pxScene->removeActors(buffer, nbActors, false);
}

bool PxSystem::Raycast(Ray& ray, RayCollision& closestCollision, CollisionLayers layers) {
	PxRaycastBuffer hit;
	PxQueryFilterData filterData = PxQueryFilterData();
	filterData.data.word0 = layers;

	const PxHitFlags outputFlags = PxHitFlag::eDEFAULT;

	pxScene->raycast(ray.GetPosition().ToPxVec3(), ray.GetDirection().ToPxVec3(), PX_MAX_F32, hit, outputFlags, filterData);

	if (hit.hasBlock) {
		closestCollision.node = static_cast<RigidBody*>(hit.block.actor->userData)->GetGameObject();
		PxVec3 position = hit.block.position;
		closestCollision.collidedAt = PxConversions::PxToVec3(position);
		closestCollision.rayDistance = hit.block.distance;
		closestCollision.collisionNormal = PxConversions::PxToVec3(hit.block.normal);
		return true;

	}
	return false;
}

bool PxSystem::CreateAggregate(vector<RigidBody*> bodies, bool selfCollisions) {
	PxAggregate* aggregate = pxPhysics->createAggregate(bodies.size(), selfCollisions);
	for (auto body : bodies) {
		if (!aggregate->addActor(*body->GetPxBody())) {
			aggregate->release(); // Failed to add all bodies to aggregate
			return false;
		}
	}
	pxScene->addAggregate(*aggregate);
	return true;
}

void PxSystem::RemoveAggregate(PxAggregate* aggregate, bool andRelease) {
	pxScene->removeAggregate(*aggregate);
	aggregate->release();
}

void PxSystem::UpdateTransforms() {

	// PhysX bug? Doesn't return anything
	/*PxU32 nbActiveActors;
	PxActor** activeActors = pxScene->getActiveActors(nbActiveActors);*/

	gameWorld.OperateOnContents(
		[](GameObject* o) {
			RigidBody* body = o->GetRigidBody();
			if (body && !body->IsStatic()) {
				PxTransform transform = body->GetPxBody()->getGlobalPose();
				
				NCLVector3 position = PxConversions::PxToVec3(transform.p);
				Quaternion orient = PxConversions::PxToQuat(transform.q);
				o->GetTransform().SetPosition(position);
				o->GetTransform().SetOrientation(orient);
			}
		}
	);
}

void PxSystem::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
	const int bufferSize = 32;
	PxContactPairPoint contacts[bufferSize];
	for (PxU32 i = 0; i < nbPairs; i++) {
		const PxContactPair& cp = pairs[i];
		if (cp.events && PxPairFlag::eCONTACT_DEFAULT) {
			ContactPoint contact;
			int nbContacts = cp.extractContacts(contacts, 32);

			contact.localA = PxConversions::PxToVec3(contacts[0].position);
			contact.localB = PxConversions::PxToVec3(contacts[0].position);
			contact.normal = PxConversions::PxToVec3(contacts[0].normal);
			contact.penetration = contacts[0].separation;

			GameObject* actor1 = reinterpret_cast<RigidBody*>(pairHeader.actors[0]->userData)->GetGameObject();
			GameObject* actor2 = reinterpret_cast<RigidBody*>(pairHeader.actors[1]->userData)->GetGameObject();
			actor1->OnCollisionBegin(actor2, contact);
			actor2->OnCollisionBegin(actor1, contact);
		}
	}
}

void PxSystem::onTrigger(PxTriggerPair* pairs, PxU32 count) {
	for (PxU32 i = 0; i < count; i++) {

		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		GameObject* otherActor = reinterpret_cast<RigidBody*>(pairs[i].otherActor->userData)->GetGameObject();
		GameObject* triggerActor = reinterpret_cast<RigidBody*>(pairs[i].triggerActor->userData)->GetGameObject();
		triggerActor->OnTrigger(otherActor);

	}
}

void PxSystem::onObjectOutOfBounds(PxShape& shape, PxActor& actor) {
	GameObject* obj = reinterpret_cast<RigidBody*>(actor.userData)->GetGameObject();
	//obj->SetToDelete(true);
}

void PxSystem::SerializeScene(bool toBinary) {
	PxSerializationRegistry* registry = PxSerialization::createSerializationRegistry(*pxPhysics);
	PxCollection* collection = PxCollectionExt::createCollection(*pxPhysics);
	PxCollection* sceneCollection = PxCollectionExt::createCollection(*pxScene);

	collection->add(*sceneCollection);
	sceneCollection->release();

	PxSerialization::complete(*collection, *registry);

	PxDefaultFileOutputStream outStream("serialized.dat");
	if (toBinary) {
		PxSerialization::serializeCollectionToBinary(outStream, *collection, *registry);
	}
	else {
		PxSerialization::serializeCollectionToXml(outStream, *collection, *registry);
	}

	collection->release();
	registry->release();
}

void PxSystem::DeserializeScene() {
	//PxColletion* collection = PxSerialization::create
}

void PxSystem::ToggleDebugMode(bool mode) {
	if (mode) {
		inDebug = true;
		pxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
		pxScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, 2.0f);
		pxScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_NORMAL, 1.0f);
	}
	else {
		inDebug = false;
		pxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 0);
	}
	
}

void PxSystem::RenderDebugInfo() {
#ifdef _WIN64
	const PxRenderBuffer& rb = pxScene->getRenderBuffer();
	for (int i = 0; i < rb.getNbLines(); i++) {
		const PxDebugLine& line = rb.getLines()[i];
		//Debug::DrawLine(PxConversions::PxToVec3(line.pos0), PxConversions::PxToVec3(line.pos1), NCLVector4::ConvertDecimalColour(line.color0));
		Debug::DrawLine(PxConversions::PxToVec3(line.pos0), PxConversions::PxToVec3(line.pos1), Debug::WHITE);
	}
	for (int i = 0; i < rb.getNbPoints(); i++) {
		const PxDebugPoint& point = rb.getPoints()[i];
		Debug::DrawLine(PxConversions::PxToVec3(point.pos), PxConversions::PxToVec3(point.pos) + NCLVector3(1, 1, 1), Debug::BLUE);
	}
	for (int i = 0; i < rb.getNbTriangles(); i++) {
		const PxDebugTriangle& tri = rb.getTriangles()[i];
		Debug::DrawLine(PxConversions::PxToVec3(tri.pos0), PxConversions::PxToVec3(tri.pos1), Debug::MAGENTA);
		Debug::DrawLine(PxConversions::PxToVec3(tri.pos1), PxConversions::PxToVec3(tri.pos2), Debug::MAGENTA);
		Debug::DrawLine(PxConversions::PxToVec3(tri.pos2), PxConversions::PxToVec3(tri.pos0), Debug::MAGENTA);
	}
#endif
}
