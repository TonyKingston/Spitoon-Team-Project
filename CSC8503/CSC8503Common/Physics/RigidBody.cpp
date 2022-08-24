#include "RigidBody.h"
#include "../Transform.h"
#include "PxSystem.h"
#include "../../Common/MeshGeometry.h"
#include "PxConversions.h"

using namespace NCL::CSC8503;
using namespace physx;


PxPhysics* RigidBody::pxp = nullptr;
PxCooking* RigidBody::cooking = nullptr;

RigidBody::RigidBody(Transform* parentTransform, bool isStatic, float sFriction, float dFriction, float restitution) {
    if (isStatic) {
        body = pxp->createRigidStatic(PxTransform(parentTransform->ToPxTransform()));
    }
    else {
        PxTransform trans = parentTransform->ToPxTransform();
        body = pxp->createRigidDynamic(PxTransform(parentTransform->ToPxTransform()));
    }

	dynamic = body->is<PxRigidDynamic>();
	material = pxp->createMaterial(sFriction, dFriction, restitution);
    transform = parentTransform;
    body->userData = this;
    filter.group = CollisionLayers::Default;
    filter.mask = CollisionLayers::Default;
}

RigidBody::~RigidBody() {
    if (body->isReleasable()) {
        body->release();
    }
}

void RigidBody::InitPhysics(PxSystem* physics) {
    RigidBody::pxp = physics->GetPhysics();
	RigidBody::cooking = physics->GetCooking();
}

void RigidBody::AddForce(const NCLVector3& force) {
    if (dynamic) {
        dynamic->addForce(force.ToPxVec3());
    }
}

void RigidBody::AddTorque(const NCLVector3& force) {
    if (dynamic) {
        dynamic->addTorque(force.ToPxVec3());
    }
}

void RigidBody::ApplyLinearImpulse(const NCLVector3& force) {
    if (dynamic) {
        dynamic->addForce(force.ToPxVec3(), physx::PxForceMode::eIMPULSE);
    }
}

void RigidBody::ApplyAngularImpulse(const NCLVector3& force) {
    if (dynamic) {
        dynamic->addTorque(force.ToPxVec3(), physx::PxForceMode::eIMPULSE);
    }
}

void RigidBody::AddForceAtPosition(const NCLVector3& force, const NCLVector3& position) {
    NCLVector3 localPos = position - transform->GetPosition();
    if (dynamic) {
        PxRigidBodyExt::addForceAtLocalPos(*dynamic, force.ToPxVec3(), localPos.ToPxVec3());
    }
}

void RigidBody::ClearForces() {
    if (dynamic) {
        dynamic->clearForce();
        dynamic->clearTorque();
    }
}

void RigidBody::SetGlobalPosition(Transform* pos) {
    if (dynamic) {
        dynamic->setGlobalPose(transform->ToPxTransform(), false);
    }
}


void NCL::CSC8503::RigidBody::AddBoxShape(const NCLVector3& halfExtents) {
    PxShape* box = PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfExtents.ToPxVec3()), *material);
	if (dynamic) {
		PxRigidBodyExt::updateMassAndInertia(*(PxRigidBody*)body, 0.05f);
	}
    AddShape(box);
}

void NCL::CSC8503::RigidBody::AddSphereShape(float radius) {
    PxShape* sphere = PxRigidActorExt::createExclusiveShape(*body, PxSphereGeometry(radius), *material);
	if (dynamic) {
		PxRigidBodyExt::updateMassAndInertia(*(PxRigidBody*)body, 0.1f);
	}
    AddShape(sphere, false);
}

void NCL::CSC8503::RigidBody::AddCapsuleShape(float radius, float halfHeight, bool upright) {
    PxShape* capsule = PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(radius, halfHeight), *material);
    if (upright) {
        PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
        capsule->setLocalPose(relativePose);
    }
	if (dynamic) {
		PxRigidBodyExt::updateMassAndInertia(*(PxRigidBody*)body, 1.0f);
	}
    AddShape(capsule);
}

void RigidBody::AddPlaneShape() {
    if (IsStatic()) {
        PxShape* plane = PxRigidActorExt::createExclusiveShape(*body, PxPlaneGeometry(), *material);
        AddShape(plane);
    }
    else {
        std::cout << "Trying to add a plane shape to a non-static rigid body." << endl;
    }
}

void RigidBody::AddConvexMesh(MeshGeometry* mesh) {
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = mesh->GetVertexCount();
	convexDesc.points.stride = sizeof(NCLVector3);
	convexDesc.points.data = mesh->GetPositionData().data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;
	if (!cooking->cookConvexMesh(convexDesc, buf, &result))
		return;
	
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());

	PxConvexMesh* convexMesh = pxp->createConvexMesh(input);
	PxMeshScale scale(transform->GetScale().ToPxVec3(), PxQuat(PxIdentity));
	PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*body,
		PxConvexMeshGeometry(convexMesh, scale), *material);
    AddShape(aConvexShape);
}

PxRigidActor* RigidBody::GetPxBody() {
    return body;
}

PxMaterial* RigidBody::GetPxMaterial() {
    return material;
}

void RigidBody::SetKinematicTarget(Transform* target) {
    if (dynamic && dynamic->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC)) {
        dynamic->setKinematicTarget(PxTransform(target->GetPosition().ToPxVec3()));
    }
}

void RigidBody::SetCollisionFilters(const CollisionLayers& filterGroup, const CollisionLayers& filterMask) {
    PxFilterData filterData;
    filterData.word0 = filterGroup;
    filterData.word1 = filterMask;
    const int numShapes = body->getNbShapes();
    for (auto shape : shapes) {
        shape->setSimulationFilterData(filterData);
        shape->setQueryFilterData(filterData);
    }
    filter.group = filterGroup;
    filter.mask = filterMask;
}

bool RigidBody::Raycast(Ray& ray, RayCollision& closestCollision, RigidBody* target) {
    PxRaycastHit hit;

    PxHitFlags outputFlags = PxHitFlag::eDEFAULT;

    bool isHit = PxGeometryQuery::raycast(ray.GetPosition().ToPxVec3(), ray.GetDirection().ToPxVec3(), target->shapes[0]->getGeometry().any(), 
        target->transform->ToPxTransform() * target->shapes[0]->getLocalPose(), PX_MAX_F32, outputFlags, 1, &hit);

    if (isHit) {
        closestCollision.node = static_cast<RigidBody*>(hit.actor->userData)->GetGameObject();
        PxVec3 position = hit.position;
        closestCollision.collidedAt = PxConversions::PxToVec3(position);
        closestCollision.rayDistance = hit.distance;
        return true;
    }
    return false;
}

inline PxTransform RigidBody::GetGlobalPose(const PxShape& shape) {
    return this->transform->ToPxTransform() * shape.getLocalPose();
}

void RigidBody::AddShape(PxShape* shape, bool debugVisualize) {
    shape->setFlag(PxShapeFlag::eVISUALIZATION, debugVisualize);
    //shape->setRestOffset(REST_OFFSET);
  //  shape->setContactOffset(CONTACT_OFFSET);
    shapes.push_back(shape);
    SetCollisionFilters(CollisionLayers::Default, CollisionLayers::Default);
}

