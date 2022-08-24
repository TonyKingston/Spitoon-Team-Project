#pragma once
#include "Transform.h"
#include "RenderObject.h"
#include <vector>
#include "Physics/RigidBody.h"
#include <functional>

using std::vector;

namespace NCL {
	namespace CSC8503 {
		class GameObject {
		public:
			GameObject(string name = "");
			~GameObject();

			bool IsActive() const {
				return isActive;
			}

			void SetIsActive(bool b) {
				isActive = b;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			RigidBody* GetRigidBody() const {
				return rigidBody;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(RigidBody* newObject) {
				rigidBody = newObject;
				rigidBody->gameObject = this;
			}

			const string& GetName() const {
				return name;
			}

			void SetName(const std::string& n) {
				this->name = n;
			}

			typedef std::function<void(GameObject*, ContactPoint)> CollisionFunc;
			virtual void OnCollisionBegin(GameObject* otherObject, ContactPoint contact) {
				if (collisionBeginFunc) {
					collisionBeginFunc(otherObject, contact);
				}
			}

			virtual void OnCollisionEnd(GameObject* otherObject, ContactPoint contact) {
				if (collisionEndFunc) {
					collisionEndFunc(otherObject, contact);
				}
			}

			typedef std::function<void(GameObject*)> TriggerFunc;
			void OnTrigger(GameObject* otherObject) {
				if (triggerFunc) {
					triggerFunc(otherObject);
				}
			}

			void SetTriggerFunc(TriggerFunc func) {
				triggerFunc = func;
			}

			void SetOnCollisionBeingFunc(CollisionFunc func) {
				collisionBeginFunc = func;
			}

			void SetOnCollisionEndFunc(CollisionFunc func) {
				collisionEndFunc = func;
			}

			void SetWorldID(int newID) {
				worldID = newID;
			}

			int	GetWorldID() const {
				return worldID;
			}
			void setGun(GameObject* gts) 
			{
				gun = gts;
			}
			GameObject* getGun()
			{
				return gun;
			}
			float getFiring()
			{
				return firing;
			}
			void setFiring(){
				firing = 60.0f;
			}
			void fire()
			{
				firing--;
			}
			int GetScore() { return score; }

			bool IsToDelete() { return toDelete; }
			void SetToDelete(bool b) { this->toDelete = b; }

			void PrintDebugInfo();

			//AI related properties
			void SetDistanceToCleaner(float distance) { distanceToCleaner = distance; }
			float GetDistanceToCleaner() { return distanceToCleaner; }

			void SetDistanceToCleaner02(float distance) { distanceToCleaner02 = distance; }
			float GetDistanceToCleaner02() { return distanceToCleaner02; }

			void SetIsPaintPlacingCube(bool b) { isPaintPlacingCube = b; }
			bool GOisPaintPlacingCube() { return isPaintPlacingCube; } // Game object is paint placing cube

		protected:
			Transform			transform;

			RigidBody* rigidBody;
			RenderObject* renderObject;

			bool	isActive;
			int		worldID;
			string	name;

			int		score;
			bool toDelete = false;

			TriggerFunc triggerFunc;
			CollisionFunc collisionBeginFunc;
			CollisionFunc collisionEndFunc;

			//AI related properties
			float distanceToCleaner = 0.0f;
			float distanceToCleaner02 = 0.0f;

			//Animation
			int currentAnim;
			float firing= 0.0f;

			// Paint
			bool isPaintPlacingCube;

			GameObject* gun;
		};
	}
}