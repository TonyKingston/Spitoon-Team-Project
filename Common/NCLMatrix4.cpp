/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "NCLMatrix4.h"
#include "NCLMatrix3.h"
#include "Maths.h"
#include "NCLVector3.h"
#include "NCLVector4.h"
#include "Quaternion.h"

using namespace NCL;
using namespace NCL::Maths;
NCLMatrix4::NCLMatrix4(void) {
	ToZero();
	array[0] = 1.0f;
	array[5] = 1.0f;
	array[10] = 1.0f;
	array[15] = 1.0f;
}

NCLMatrix4::NCLMatrix4(float elements[16]) {
	memcpy(this->array, elements, 16 * sizeof(float));
}

NCLMatrix4::NCLMatrix4(const NCLMatrix3& m3) {
	ToZero();
	array[0] = m3.array[0];
	array[1] = m3.array[1];
	array[2] = m3.array[2];

	array[4] = m3.array[3];
	array[5] = m3.array[4];
	array[6] = m3.array[5];

	array[8] = m3.array[6];
	array[9] = m3.array[7];
	array[10] = m3.array[8];

	array[15] = 1.0f;
}

NCLMatrix4::NCLMatrix4(const Quaternion& quat) : NCLMatrix4() {
	float yy = quat.y * quat.y;
	float zz = quat.z * quat.z;
	float xy = quat.x * quat.y;
	float zw = quat.z * quat.w;
	float xz = quat.x * quat.z;
	float yw = quat.y * quat.w;
	float xx = quat.x * quat.x;
	float yz = quat.y * quat.z;
	float xw = quat.x * quat.w;

	array[0] = 1 - 2 * yy - 2 * zz;
	array[1] = 2 * xy + 2 * zw;
	array[2] = 2 * xz - 2 * yw;

	array[4] = 2 * xy - 2 * zw;
	array[5] = 1 - 2 * xx - 2 * zz;
	array[6] = 2 * yz + 2 * xw;

	array[8] = 2 * xz + 2 * yw;
	array[9] = 2 * yz - 2 * xw;
	array[10] = 1 - 2 * xx - 2 * yy;
}

NCLMatrix4::~NCLMatrix4(void) {
}

void NCLMatrix4::ToZero() {
	for (int i = 0; i < 16; i++) {
		array[i] = 0.0f;
	}
}

void NCLMatrix4::ToIdentity() {
	ToZero();
	array[0] = 1.0f;
	array[5] = 1.0f;
	array[10] = 1.0f;
	array[15] = 1.0f;
}

physx::PxMat44 NCLMatrix4::ToPxMat() const {
	return physx::PxMat44(GetColumn(0).ToPxVec4(), GetColumn(1).ToPxVec4(),
		GetColumn(2).ToPxVec4(), GetColumn(3).ToPxVec4());
}

NCLVector3 NCLMatrix4::GetPositionNCLVector() const {
	return NCLVector3(array[12], array[13], array[14]);
}

void	NCLMatrix4::SetPositionNCLVector(const NCLVector3& in) {
	array[12] = in.x;
	array[13] = in.y;
	array[14] = in.z;
}

NCLVector3 NCLMatrix4::GetDiagonal() const {
	return NCLVector3(array[0], array[5], array[10]);
}

void	NCLMatrix4::SetDiagonal(const NCLVector3& in) {
	array[0] = in.x;
	array[5] = in.y;
	array[10] = in.z;
}

NCLMatrix4 NCLMatrix4::Perspective(float znear, float zfar, float aspect, float fov) {
	NCLMatrix4 m;

	const float h = 1.0f / tan(fov * Maths::PI_OVER_360);
	float neg_depth = znear - zfar;

	m.array[0] = h / aspect;
	m.array[5] = h;
	m.array[10] = (zfar + znear) / neg_depth;
	m.array[11] = -1.0f;
	m.array[14] = 2.0f * (znear * zfar) / neg_depth;
	m.array[15] = 0.0f;

	return m;
}

//http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
NCLMatrix4 NCLMatrix4::Orthographic(float znear, float zfar, float right, float left, float top, float bottom) {
	NCLMatrix4 m;

	m.array[0] = 2.0f / (right - left);
	m.array[5] = 2.0f / (top - bottom);
	m.array[10] = -2.0f / (zfar - znear);

	m.array[12] = -(right + left) / (right - left);
	m.array[13] = -(top + bottom) / (top - bottom);
	m.array[14] = -(zfar + znear) / (zfar - znear);
	m.array[15] = 1.0f;

	return m;
}

NCLMatrix4 NCLMatrix4::BuildViewMatrix(const NCLVector3& from, const NCLVector3& lookingAt, const NCLVector3& up) {
	NCLMatrix4 r;
	r.SetPositionNCLVector(NCLVector3(-from.x, -from.y, -from.z));

	NCLMatrix4 m;

	NCLVector3 f = (lookingAt - from);
	f.Normalise();

	NCLVector3 s = NCLVector3::Cross(f, up).Normalised();
	NCLVector3 u = NCLVector3::Cross(s, f).Normalised();

	m.array[0] = s.x;
	m.array[4] = s.y;
	m.array[8] = s.z;

	m.array[1] = u.x;
	m.array[5] = u.y;
	m.array[9] = u.z;

	m.array[2] = -f.x;
	m.array[6] = -f.y;
	m.array[10] = -f.z;

	return m * r;
}

NCLMatrix4 NCLMatrix4::Rotation(float degrees, const NCLVector3& inaxis) {
	NCLMatrix4 m;

	NCLVector3 axis = inaxis;

	axis.Normalise();

	float c = cos((float)Maths::DegreesToRadians(degrees));
	float s = sin((float)Maths::DegreesToRadians(degrees));

	m.array[0] = (axis.x * axis.x) * (1.0f - c) + c;
	m.array[1] = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
	m.array[2] = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

	m.array[4] = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
	m.array[5] = (axis.y * axis.y) * (1.0f - c) + c;
	m.array[6] = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

	m.array[8] = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
	m.array[9] = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
	m.array[10] = (axis.z * axis.z) * (1.0f - c) + c;

	return m;
}

NCLMatrix4 NCLMatrix4::Scale(const NCLVector3& scale) {
	NCLMatrix4 m;

	m.array[0] = scale.x;
	m.array[5] = scale.y;
	m.array[10] = scale.z;

	return m;
}

NCLMatrix4 NCLMatrix4::Translation(const NCLVector3& translation) {
	NCLMatrix4 m;

	m.array[12] = translation.x;
	m.array[13] = translation.y;
	m.array[14] = translation.z;

	return m;
}

//Yoinked from the Open Source Doom 3 release - all credit goes to id software!
void    NCLMatrix4::Invert() {
	float det, invDet;

	// 2x2 sub-determinants required to calculate 4x4 determinant
	float det2_01_01 = array[0] * array[5] - array[1] * array[4];
	float det2_01_02 = array[0] * array[6] - array[2] * array[4];
	float det2_01_03 = array[0] * array[7] - array[3] * array[4];
	float det2_01_12 = array[1] * array[6] - array[2] * array[5];
	float det2_01_13 = array[1] * array[7] - array[3] * array[5];
	float det2_01_23 = array[2] * array[7] - array[3] * array[6];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	float det3_201_012 = array[8] * det2_01_12 - array[9] * det2_01_02 + array[10] * det2_01_01;
	float det3_201_013 = array[8] * det2_01_13 - array[9] * det2_01_03 + array[11] * det2_01_01;
	float det3_201_023 = array[8] * det2_01_23 - array[10] * det2_01_03 + array[11] * det2_01_02;
	float det3_201_123 = array[9] * det2_01_23 - array[10] * det2_01_13 + array[11] * det2_01_12;

	det = (-det3_201_123 * array[12] + det3_201_023 * array[13] - det3_201_013 * array[14] + det3_201_012 * array[15]);

	invDet = 1.0f / det;

	// remaining 2x2 sub-determinants
	float det2_03_01 = array[0] * array[13] - array[1] * array[12];
	float det2_03_02 = array[0] * array[14] - array[2] * array[12];
	float det2_03_03 = array[0] * array[15] - array[3] * array[12];
	float det2_03_12 = array[1] * array[14] - array[2] * array[13];
	float det2_03_13 = array[1] * array[15] - array[3] * array[13];
	float det2_03_23 = array[2] * array[15] - array[3] * array[14];

	float det2_13_01 = array[4] * array[13] - array[5] * array[12];
	float det2_13_02 = array[4] * array[14] - array[6] * array[12];
	float det2_13_03 = array[4] * array[15] - array[7] * array[12];
	float det2_13_12 = array[5] * array[14] - array[6] * array[13];
	float det2_13_13 = array[5] * array[15] - array[7] * array[13];
	float det2_13_23 = array[6] * array[15] - array[7] * array[14];

	// remaining 3x3 sub-determinants
	float det3_203_012 = array[8] * det2_03_12 - array[9] * det2_03_02 + array[10] * det2_03_01;
	float det3_203_013 = array[8] * det2_03_13 - array[9] * det2_03_03 + array[11] * det2_03_01;
	float det3_203_023 = array[8] * det2_03_23 - array[10] * det2_03_03 + array[11] * det2_03_02;
	float det3_203_123 = array[9] * det2_03_23 - array[10] * det2_03_13 + array[11] * det2_03_12;

	float det3_213_012 = array[8] * det2_13_12 - array[9] * det2_13_02 + array[10] * det2_13_01;
	float det3_213_013 = array[8] * det2_13_13 - array[9] * det2_13_03 + array[11] * det2_13_01;
	float det3_213_023 = array[8] * det2_13_23 - array[10] * det2_13_03 + array[11] * det2_13_02;
	float det3_213_123 = array[9] * det2_13_23 - array[10] * det2_13_13 + array[11] * det2_13_12;

	float det3_301_012 = array[12] * det2_01_12 - array[13] * det2_01_02 + array[14] * det2_01_01;
	float det3_301_013 = array[12] * det2_01_13 - array[13] * det2_01_03 + array[15] * det2_01_01;
	float det3_301_023 = array[12] * det2_01_23 - array[14] * det2_01_03 + array[15] * det2_01_02;
	float det3_301_123 = array[13] * det2_01_23 - array[14] * det2_01_13 + array[15] * det2_01_12;

	array[0] = -det3_213_123 * invDet;
	array[4] = +det3_213_023 * invDet;
	array[8] = -det3_213_013 * invDet;
	array[12] = +det3_213_012 * invDet;

	array[1] = +det3_203_123 * invDet;
	array[5] = -det3_203_023 * invDet;
	array[9] = +det3_203_013 * invDet;
	array[13] = -det3_203_012 * invDet;

	array[2] = +det3_301_123 * invDet;
	array[6] = -det3_301_023 * invDet;
	array[10] = +det3_301_013 * invDet;
	array[14] = -det3_301_012 * invDet;

	array[3] = -det3_201_123 * invDet;
	array[7] = +det3_201_023 * invDet;
	array[11] = -det3_201_013 * invDet;
	array[15] = +det3_201_012 * invDet;
}

NCLMatrix4 NCLMatrix4::Inverse()	const {
	NCLMatrix4 temp(*this);
	temp.Invert();
	return temp;
}

NCLVector4 NCLMatrix4::GetRow(unsigned int row) const {
	NCLVector4 out(0, 0, 0, 1);
	if (row <= 3) {
		int start = row;

		out.x = array[start];
		out.y = array[start + 4];
		out.z = array[start + 8];
		out.w = array[start + 12];
	}
	return out;
}

NCLVector4 NCLMatrix4::GetColumn(unsigned int column) const {
	NCLVector4 out(0, 0, 0, 1);

	if (column <= 3) {
		memcpy(&out, &array[4 * column], sizeof(NCLVector4));
	}

	return out;
}

NCLMatrix4 NCL::Maths::NCLMatrix4::Lerp(const NCLMatrix4& from, const NCLMatrix4& to, float p) {
	NCLVector3 lerpedPos = NCLVector3::Lerp(from.GetPositionNCLVector(), to.GetPositionNCLVector(), p);

	NCLMatrix4 temp = from;
	NCLMatrix4 temp2 = to;
	temp = temp * Quaternion(temp).Conjugate();
	temp2 = temp2 * Quaternion(temp2).Conjugate();

	NCLVector3 lerpedScale = NCLVector3::Lerp(temp.GetDiagonal(), temp2.GetDiagonal(), p);
	Quaternion lerpedRot = Quaternion::Slerp(from, to, p);

	NCLMatrix4 lerpedMat = NCLMatrix4::Translation(lerpedPos) * NCLMatrix4::Scale(lerpedScale) * NCLMatrix4(lerpedRot);

	return lerpedMat;
}

NCLVector3 NCLMatrix4::operator*(const NCLVector3& v) const {
	NCLVector3 vec;

	float temp;

	vec.x = v.x * array[0] + v.y * array[4] + v.z * array[8] + array[12];
	vec.y = v.x * array[1] + v.y * array[5] + v.z * array[9] + array[13];
	vec.z = v.x * array[2] + v.y * array[6] + v.z * array[10] + array[14];

	temp = v.x * array[3] + v.y * array[7] + v.z * array[11] + array[15];

	vec.x = vec.x / temp;
	vec.y = vec.y / temp;
	vec.z = vec.z / temp;

	return vec;
}

NCLVector4 NCLMatrix4::operator*(const NCLVector4& v) const {
	return NCLVector4(
		v.x * array[0] + v.y * array[4] + v.z * array[8] + v.w * array[12],
		v.x * array[1] + v.y * array[5] + v.z * array[9] + v.w * array[13],
		v.x * array[2] + v.y * array[6] + v.z * array[10] + v.w * array[14],
		v.x * array[3] + v.y * array[7] + v.z * array[11] + v.w * array[15]
	);
}
