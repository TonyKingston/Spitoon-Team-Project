#include "GameObject.h"
#include "Physics/CollisionDetection.h"
#include "Debug.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName) {
	name = objectName;
	worldID = -1;
	isActive = true;
	isPaintPlacingCube = false;
	rigidBody = nullptr;
	renderObject = nullptr;
}

GameObject::~GameObject() {
	delete rigidBody;
	delete renderObject;
}

void GameObject::PrintDebugInfo() {
#ifdef _x64
	/*
	NCLVector4 colour = Debug::BLACK;
	Debug::Print("Debug info for " + name + ", World ID: " + std::to_string(worldID), NCLVector2(1.0f, 3.0f), colour);
	Debug::Print("POSITION", NCLVector2(1.0f, 6.0f), colour);
	Debug::Print("x: " + std::to_string(transform.GetPosition().x), NCLVector2(1.0f, 9.0f), colour);
	Debug::Print("y: " + std::to_string(transform.GetPosition().y), NCLVector2(1.0f, 12.0f));
	Debug::Print("z: " + std::to_string(transform.GetPosition().z), NCLVector2(1.0f, 15.0f));
	Debug::Print("ROTATION", NCLVector2(30.0f, 6.0f));
	Debug::Print("x: " + std::to_string(transform.GetOrientation().ToEuler().x), NCLVector2(30.0f, 9.0f), colour);
	Debug::Print("y: " + std::to_string(transform.GetOrientation().ToEuler().y), NCLVector2(30.0f, 12.0f), colour);
	Debug::Print("z: " + std::to_string(transform.GetOrientation().ToEuler().z), NCLVector2(30.0f, 15.0f), colour);
	Debug::Print("SCALE", NCLVector2(60.0f, 6.0f));
	Debug::Print("x: " + std::to_string(transform.GetScale().x), NCLVector2(60.0f, 9.0f), colour);
	Debug::Print("y: " + std::to_string(transform.GetScale().y), NCLVector2(60.0f, 12.0f), colour);
	Debug::Print("z: " + std::to_string(transform.GetScale().z), NCLVector2(60.0f, 15.0f), colour);

	Debug::Print("LIN. VELOCITY: ", NCLVector2(1.0f, 18.0f));
	Debug::Print("x: " + std::to_string(rigidBody->GetLinearVelocity().x), NCLVector2(1.0f, 21.0f));
	Debug::Print("y: " + std::to_string(rigidBody->GetLinearVelocity().x), NCLVector2(1.0f, 24.0f));
	Debug::Print("z: " + std::to_string(rigidBody->GetLinearVelocity().x), NCLVector2(1.0f, 27.0f));

	Debug::Print("ANG. VELOCITY: ", NCLVector2(30.0f, 18.0f));
	Debug::Print("x: " + std::to_string(rigidBody->GetAngularVelocity().x), NCLVector2(30.0f, 21.0f));
	Debug::Print("y: " + std::to_string(rigidBody->GetAngularVelocity().x), NCLVector2(30.0f, 24.0f));
	Debug::Print("z: " + std::to_string(rigidBody->GetAngularVelocity().x), NCLVector2(30.0f, 27.0f));

	float mass = 1 / rigidBody->GetInverseMass();
	(mass < std::numeric_limits<float>::infinity()) ? Debug::Print("Mass: " + std::to_string(mass), NCLVector2(60.0f, 18.0f), colour) :
		Debug::Print("Mass: Infinite (0)", NCLVector2(60.0f, 18.0f), colour);
		*/
#endif _x64
}
