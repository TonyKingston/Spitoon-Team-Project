#include "ColliderComponent.h"

using namespace NCL::CSC8503;

NCL::CSC8503::ColliderComponent::ColliderComponent(PxGeometry& geometry, PxMaterial& material) {
}

void ColliderComponent::SetTrigger(bool isTrigger) {
    this->isTrigger = isTrigger;

    if (shape) {
        if (isTrigger) {
            shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
            shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
        }
        else {
            shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
            shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
        }
    }
}
