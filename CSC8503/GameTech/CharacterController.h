#include "../CSC8503Common/GameWorld.h"
#include "../CSC8503Common/Physics/PxSystem.h"
#include "PlayerObject.h"

#ifdef _WIN64
namespace NCL {
	class CharacterController {
	public:

		enum moveDirection {left, right, forward, backward, idle};

		CharacterController(GameWorld* world, PlayerObject* playerObject, int playerNum, PxSystem* physics);

		void PlayerMovement(bool isSplitScreen);
		void MoveDirection(moveDirection direction, NCLVector3 axis);
		bool isGrounded();
		bool StopMove();
		void Jump();
		void CheckAnimationState();
		static void CheckAnimationStateNet(GameObject* playerObject, moveDirection direction);

		void CheckCollisions();
		void CheckDash();
		void Dash();

		moveDirection facingDirection = NCL::CharacterController::moveDirection::idle;
		moveDirection previousFacingDirection;

		PxSystem* physics;
		PlayerObject* playerObject;
		GameWorld* world;
		int playerNum, jumpCount;
		float moveForce, jumpForce, dashTimeLeft, dashTime, dashSpeed, speedBoostLeft, jumpControllerCooldown, dashControllerCooldown;
		bool grounded, doubleJumpCheck, isDashing, isFiring, isMoving, wasMoving, canDash, speedBoost, controllerCanJump, controllerCanDash;
		int count, dashCount;
		float amount_time = 0, airMovementMultiplier;
		NCLVector3 pre_postition, rightAxis, fwdAxis;

	};
}
#endif _WIN64;