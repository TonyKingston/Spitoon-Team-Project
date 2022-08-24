#include "Transform.h"

using namespace NCL::CSC8503;

Transform::Transform()
{
	scale	= NCLVector3(1, 1, 1);
}

Transform::~Transform()
{

}

void Transform::UpdateMatrix() {
	matrix =
		NCLMatrix4::Translation(position) *
		NCLMatrix4(orientation) *
		NCLMatrix4::Scale(scale);
}

Transform& Transform::SetPosition(const NCLVector3& worldPos) {
	position = worldPos;
	UpdateMatrix();
	return *this;
}

Transform& Transform::SetScale(const NCLVector3& worldScale) {
	scale = worldScale;
	UpdateMatrix();
	return *this;
}

Transform& Transform::SetOrientation(const Quaternion& worldOrientation) {
	orientation = worldOrientation;
	UpdateMatrix();
	return *this;
}

NCLMatrix4 Transform::Lerp(const NCLMatrix4& to, float p) {
	NCLVector3 lerpedPos = NCLVector3::Lerp(position, to.GetPositionNCLVector(), p);

	NCLMatrix4 temp = to;
	temp = temp * Quaternion(temp).Conjugate();
	NCLVector3 lerpedScale = NCLVector3::Lerp(scale, temp.GetDiagonal(), p);
	Quaternion lerpedRot = Quaternion::Slerp(orientation, to, p);

	NCLMatrix4 lerpedMat = NCLMatrix4::Translation(lerpedPos) * NCLMatrix4::Scale(lerpedScale) * NCLMatrix4(lerpedRot);

	return lerpedMat;
}