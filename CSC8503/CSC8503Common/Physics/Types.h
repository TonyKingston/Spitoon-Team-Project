#pragma once
#include "../../Common/NCLVector3.h"

struct RigidBodyConstraints {
	enum Enum {
		None = 0,
		LockPosX = 1 << 0,
		LockPosY = 1 << 1,
		LockPosZ = 1 << 2,
		LockRotX = 1 << 3,
		LockRotY = 1 << 4,
		LockRotZ = 1 << 5,
		LockPos = LockPosX | LockPosY | LockPosZ,
		LockRot = LockRotX | LockRotY | LockRotZ,
		LockAll = LockPos | LockRot
	};

};

enum CollisionLayers {
	Default = (1 << 0),
	Group1 = (1 << 1),
	Group2 = (1 << 2),
	Paint = (1 << 3),
	Group4 = (1 << 4),
	Group5 = (1 << 5),
	Group6 = (1 << 6),
	Group7 = (1 << 7),
	Group8 = (1 << 8),
	Group9 = (1 << 9),
	ALL = Default | Group1 | Group2 | Paint | Group4 | Group5 | Group6 | Group7
};

inline CollisionLayers operator|(CollisionLayers a, CollisionLayers b) {
	return static_cast<CollisionLayers>(static_cast<int>(a) | static_cast<int>(b));
}

struct PhysicsMaterial {
	float sFriction = 0.5;
	float dFriction = 0.5;
	float resitution = 0.5f;
};

struct RigidBodyParams {
	int mass = 1.0f;
	float sFriction = 0.5;
	float dFriction = 0.5;
	float restitution = 0.5f;
	bool isStatic = false;
	bool isKinematic = false;
};

struct ContactPoint {
	NCL::Maths::NCLVector3 localA;
	NCL::Maths::NCLVector3 localB;
	NCL::Maths::NCLVector3 normal;
	float	penetration;
};