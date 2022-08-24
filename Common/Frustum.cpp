#include "Frustum.h"
#include "NCLMatrix4.h"
#include "../CSC8503/CSC8503Common/RenderObject.h"
#include "../CSC8503/CSC8503Common/Transform.h"

using namespace NCL::CSC8503;

void NCL::Rendering::Frustum::FromMatrix(const NCLMatrix4& mat) {
	NCLVector3 xaxis = NCLVector3(mat.array[0], mat.array[4], mat.array[8]);
	NCLVector3 yaxis = NCLVector3(mat.array[1], mat.array[5], mat.array[9]);
	NCLVector3 zaxis = NCLVector3(mat.array[2], mat.array[6], mat.array[10]);
	NCLVector3 waxis = NCLVector3(mat.array[3], mat.array[7], mat.array[11]);

	// RIGHT
	planes[0] = Plane(waxis - xaxis,
		(mat.array[15] - mat.array[12]), true);
	// LEFT
	planes[1] = Plane(waxis + xaxis,
		(mat.array[15] + mat.array[12]), true);
	// BOTTOM
	planes[2] = Plane(waxis + yaxis,
		(mat.array[15] + mat.array[13]), true);
	// TOP
	planes[3] = Plane(waxis - yaxis,
		(mat.array[15] - mat.array[13]), true);
	// NEAR
	planes[4] = Plane(waxis + zaxis,
		(mat.array[15] + mat.array[14]), true);
	// FAR
	planes[5] = Plane(waxis - zaxis,
		(mat.array[15] - mat.array[14]), true);
}

bool NCL::Rendering::Frustum::InsideFrustum(RenderObject& n) {
	for (int p = 0; p < 6; ++p) {
		if (!planes[p].SphereInPlane(n.GetTransform()->GetPosition(), n.GetBoundingRadius())) {
			return false; // RenderObject is outside this plane.

		}
	}
	return true;
}
