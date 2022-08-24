#include "CharacterController.h"
#ifdef _WIN64
#include <SFML\Window\Joystick.hpp>
#include "Audio.h"
#include "../../Common/Window.h"


NCL::CharacterController::CharacterController(GameWorld* world, PlayerObject* playerObject, int playerNum, PxSystem* physics)
{
	this->world = world;
	this->playerObject = playerObject;
	this->playerNum = playerNum;
	this->physics = physics;
	this->pre_postition = playerObject->GetTransform().GetPosition();

	grounded = false;
	doubleJumpCheck = false;
	isMoving = false;
	canDash = true;
	speedBoost = false;

	speedBoostLeft = 10.0f;
	dashCount = 2;
	dashSpeed = 50.0f;
	dashTime = 0.2f;
	moveForce = 200.0f;
	jumpForce = 100.0f;
	count = 0;

	dashControllerCooldown = 1.0f;
	jumpControllerCooldown = 4.0f;
}

void NCL::CharacterController::PlayerMovement(bool isSplitScreen)
{
	if (speedBoost) {
		if (speedBoostLeft >= 0) {
			moveForce = 400.0f;
			speedBoostLeft -= Window::GetTimer()->GetTimeDeltaSeconds();
		}
		else {
			speedBoost = false;
			speedBoostLeft = 10.0f;
		}
	}
	else {
		moveForce = 200.0f;
	}

	if (!controllerCanJump) {
		if (jumpControllerCooldown <= 0) {
			controllerCanJump = true;
		}
		else {
			jumpControllerCooldown -= Window::GetTimer()->GetTimeDeltaSeconds();
		}
	}
	
	

	if (dashControllerCooldown <= 0) {
		controllerCanDash = true;
	}
	else {
		dashControllerCooldown -= Window::GetTimer()->GetTimeDeltaSeconds();
	}

	auto PlayJumpAudio = [](bool isground, bool doublecheck, const int& player) {
		if (isground | doublecheck) {
#ifdef _WIN64
			audio::PlayJump(player);
#endif _WIN64
		}
	};

	NCLMatrix4 view;

	Quaternion quat;
	switch (playerNum)
	{
	case 1:
		view = world->GetMainCamera()->BuildViewMatrix();
		quat = Quaternion::EulerAnglesToQuaternion(0, world->GetMainCamera()->GetYaw(), 0);
		playerObject->GetRigidBody()->SetGlobalPosition(&playerObject->GetTransform().SetOrientation(quat));
		break;
	case 2:
		if (isSplitScreen) {
			view = world->GetViceCamera()->BuildViewMatrix();
			quat = Quaternion::EulerAnglesToQuaternion(0, world->GetViceCamera()->GetYaw(), 0);
			playerObject->GetRigidBody()->SetGlobalPosition(&playerObject->GetTransform().SetOrientation(quat));
		}
		else {
			view = world->GetMainCamera()->BuildViewMatrix();
			quat = Quaternion::EulerAnglesToQuaternion(0, world->GetMainCamera()->GetYaw(), 0);
			playerObject->GetRigidBody()->SetGlobalPosition(&playerObject->GetTransform().SetOrientation(quat));
		}
	default:
		break;
	}

	NCLMatrix4 camWorld = view.Inverse();
	rightAxis = NCLVector3(camWorld.GetColumn(0));

	fwdAxis = NCLVector3::Cross(NCLVector3(0.0f, 1.0f, 0.0f), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	// Controller
	float x;
	float y;
	float z;
#ifdef _WIN64
	sf::Joystick::update();
	x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
	y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
	z = sf::Joystick::getAxisPosition(0, sf::Joystick::Z);
#endif _WIN64

	if (isSplitScreen) {
		int time = Window::GetTimer()->GetTimeDeltaSeconds();
		switch (playerNum)
		{
		case 1:
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
				MoveDirection(NCL::CharacterController::moveDirection::left, rightAxis);			}

			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
				MoveDirection(NCL::CharacterController::moveDirection::right, rightAxis);			}

			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
				MoveDirection(NCL::CharacterController::moveDirection::forward, fwdAxis);
			}

			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
				MoveDirection(NCL::CharacterController::moveDirection::backward, fwdAxis);
			}

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
				PlayJumpAudio(isGrounded(), doubleJumpCheck, playerNum - 1);
				Jump();
			}

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SHIFT)) {
				Dash();
			}
			break;
		case 2:
			if (x < -10) {
				MoveDirection(NCL::CharacterController::moveDirection::left, rightAxis);
			}

			if (x > 10) {
				MoveDirection(NCL::CharacterController::moveDirection::right, rightAxis);
			}

			if (y < -10) {
				MoveDirection(NCL::CharacterController::moveDirection::forward, fwdAxis);
			}

			if (y > 10) {
				MoveDirection(NCL::CharacterController::moveDirection::backward, fwdAxis);
			}
#ifdef _WIN64

			if (sf::Joystick::isButtonPressed(0, 1)) {
				if (controllerCanJump) {
					PlayJumpAudio(isGrounded(), doubleJumpCheck, playerNum - 1);
					controllerCanJump = false;
					if (jumpCount < 2) {
						Jump();
					}
					else {
						jumpCount = 0;
					}
					jumpCount++;
				}
			}
			if (sf::Joystick::isButtonPressed(0, 2) && (time % 2 == 0)) {
				if (controllerCanDash) {
					PlayJumpAudio(isGrounded(), doubleJumpCheck, playerNum - 1);
					controllerCanDash = false;
					if (jumpCount < 2) {
						Dash();
					}
					else {
						jumpCount = 0;
					}
					jumpCount++;
				}
			}
#endif _WIN64
			break;
		default:
			break;
		}
	}
	else {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A) || x < -10) {
			MoveDirection(NCL::CharacterController::moveDirection::left, rightAxis);

		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D) || x > 10) {
			MoveDirection(NCL::CharacterController::moveDirection::right, rightAxis);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W) || y < -10) {
			MoveDirection(NCL::CharacterController::moveDirection::forward, fwdAxis);

		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S) || y > 10) {
			MoveDirection(NCL::CharacterController::moveDirection::backward, fwdAxis);
		}
#ifdef _WIN64
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) || sf::Joystick::isButtonPressed(0, 1)) {
			if (controllerCanJump) {
				PlayJumpAudio(isGrounded(), doubleJumpCheck, playerNum - 1);
				controllerCanJump = false;
				if (jumpCount < 2) {
					Jump();
				}
				else {
					jumpCount = 0;
				}
				jumpCount++;
			}
		}
#endif _WIN64
		int time = Window::GetTimer()->GetTimeDeltaSeconds();
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SHIFT) || ((sf::Joystick::isButtonPressed(0, 2)) && (time % 2 == 0))) {
			if (controllerCanDash) {
				controllerCanDash = false;
				if (jumpCount < 2) {
					Dash();
				}
				else {
					jumpCount = 0;
				}
				jumpCount++;
			}
		}
	}

	if (amount_time > 0.35) {
		pre_postition = playerObject->GetTransform().GetPosition();
		if (StopMove()) {
#ifdef _WIN64
			audio::StopStep(playerNum - 1);
			amount_time = 0;
#endif _WIN64
		}
	}
	amount_time += Window::GetTimer()->GetTimeDeltaSeconds();


	if (playerObject->GetRigidBody()->GetLinearVelocity().Length() != NULL) {
		if (playerObject->GetRigidBody()->GetLinearVelocity().Length() > 2.0f) {
			wasMoving = isMoving;
			isMoving = true;
		}
		else {
			isMoving = false;
		}
	}

	count++;
	if (!isMoving) {
		facingDirection = NCL::CharacterController::moveDirection::idle;
	}
	if (playerObject->getFiring()<=0)
	{
		CheckAnimationState();
	}
	
	CheckCollisions();
	CheckDash();
}

void NCL::CharacterController::MoveDirection(moveDirection direction, NCLVector3 axis)
{
#ifdef _WIN64
 	if (isGrounded())
		audio::PlayStep(playerNum - 1);
#endif _WIN64
	previousFacingDirection = facingDirection;
	switch (direction)
	{
	case NCL::CharacterController::moveDirection::left:
		if (isGrounded()) {
			playerObject->GetRigidBody()->AddForce(-axis * moveForce);
			facingDirection = NCL::CharacterController::moveDirection::left;
		}
		else {
			playerObject->GetRigidBody()->AddForce(-axis * moveForce / 2);
		}
		break;
	case NCL::CharacterController::moveDirection::right:
		if (isGrounded()) {
			playerObject->GetRigidBody()->AddForce(axis * moveForce);
			facingDirection = NCL::CharacterController::moveDirection::right;
		}
		else {
			playerObject->GetRigidBody()->AddForce(axis * moveForce / 2);
		}
		break;
	case NCL::CharacterController::moveDirection::forward:
		if (isGrounded()) {
			playerObject->GetRigidBody()->AddForce(axis * moveForce);
			facingDirection = NCL::CharacterController::moveDirection::forward;
		}
		else {
			playerObject->GetRigidBody()->AddForce(axis * moveForce / 2);
		}
		break;
	case NCL::CharacterController::moveDirection::backward:
		if (isGrounded()) {
			playerObject->GetRigidBody()->AddForce(-axis * moveForce);
			facingDirection = NCL::CharacterController::moveDirection::backward;
		}
		else {
			playerObject->GetRigidBody()->AddForce(-axis * moveForce / 2);
		}
		break;
	default:
		break;
	}
}

bool NCL::CharacterController::isGrounded()
{
	grounded = false;
	Ray ray = Ray(playerObject->GetTransform().GetPosition(), NCLVector3(0.0f, -1.0f, 0.0f));

	RayCollision closestCollision;
	physics->Raycast(ray, closestCollision);

	if (closestCollision.rayDistance < 3.1f) {
		grounded = true;
		doubleJumpCheck = true;
	}
	else {
		return false;
	}
}

void NCL::CharacterController::Jump()
{
	if (isGrounded()) {
		grounded = true;
		doubleJumpCheck = true;
		playerObject->GetRigidBody()->ApplyLinearImpulse((NCLVector3(0, 1, 0) * jumpForce));
	}
	else if (doubleJumpCheck) {
		playerObject->GetRigidBody()->ApplyLinearImpulse((NCLVector3(0, 1, 0) * jumpForce));
		doubleJumpCheck = false;
	}
}

void NCL::CharacterController::CheckAnimationState()
{
	if (isMoving) {
		if (previousFacingDirection != facingDirection) {
			switch (facingDirection)
			{
			case NCL::CharacterController::moveDirection::left:
				playerObject->GetRenderObject()->SetAnimation(playerObject->GetRenderObject()->animList[5]);
				playerObject->getGun()->SetIsActive(true);
				break;
			case NCL::CharacterController::moveDirection::right:
				playerObject->GetRenderObject()->SetAnimation(playerObject->GetRenderObject()->animList[4]);
				playerObject->getGun()->SetIsActive(true);
				break;
			case NCL::CharacterController::moveDirection::forward:
				playerObject->GetRenderObject()->SetAnimation(playerObject->GetRenderObject()->animList[3]);
				playerObject->getGun()->SetIsActive(true);
				break;
			case NCL::CharacterController::moveDirection::backward:
				playerObject->GetRenderObject()->SetAnimation(playerObject->GetRenderObject()->animList[2]);
				playerObject->getGun()->SetIsActive(true);
				break;
			default:
				break;
			}
		}
	}
	else {
		playerObject->GetRenderObject()->SetAnimation(playerObject->GetRenderObject()->animList[0]);
		playerObject->getGun()->SetIsActive(false);
	}
	if (playerObject->GetRenderObject()->GetCurrentFrame() >= 34) {
		playerObject->GetRenderObject()->ResetCurrentFrame();
	}
}

void NCL::CharacterController::CheckAnimationStateNet(GameObject* playerObject, moveDirection direction)
{
	switch (direction)
	{
	case NCL::CharacterController::moveDirection::left:
		playerObject->GetRenderObject()->SetAnimation(playerObject->GetRenderObject()->animList[5]);
		playerObject->getGun()->SetIsActive(true);
		break;
	case NCL::CharacterController::moveDirection::right:
		playerObject->GetRenderObject()->SetAnimation(playerObject->GetRenderObject()->animList[4]);
		playerObject->getGun()->SetIsActive(true);
		break;
	case NCL::CharacterController::moveDirection::forward:
		playerObject->GetRenderObject()->SetAnimation(playerObject->GetRenderObject()->animList[3]);
		playerObject->getGun()->SetIsActive(true);
		break;
	case NCL::CharacterController::moveDirection::backward:
		playerObject->GetRenderObject()->SetAnimation(playerObject->GetRenderObject()->animList[2]);
		playerObject->getGun()->SetIsActive(true);
		break;
	default:
		playerObject->GetRenderObject()->SetAnimation(playerObject->GetRenderObject()->animList[0]);
		playerObject->getGun()->SetIsActive(false);
		break;
	}
	if (playerObject->GetRenderObject()->GetCurrentFrame() >= 34) {
		playerObject->GetRenderObject()->ResetCurrentFrame();
	}
}

void NCL::CharacterController::CheckCollisions()
{
	if (playerObject->dashCollided) {
		canDash = true;
		playerObject->dashCollided = false;
	}
	if (playerObject->speedCollided) {
		speedBoost = true;
		playerObject->speedCollided = false;
	}
}

void NCL::CharacterController::CheckDash()
{
	if (isDashing) {
		if (dashTimeLeft > 0) {
			playerObject;
			dashTimeLeft -= Window::GetTimer()->GetTimeDeltaSeconds();
			switch (facingDirection)
			{
			case NCL::CharacterController::left:
				if (isGrounded()) {
					playerObject->GetRigidBody()->SetLinearVelocity(-rightAxis * dashSpeed);
				}
				break;
			case NCL::CharacterController::right:
				if (isGrounded()) {
					playerObject->GetRigidBody()->SetLinearVelocity(rightAxis * dashSpeed);
				}
				break;
			case NCL::CharacterController::forward:
				if (isGrounded()) {
					playerObject->GetRigidBody()->SetLinearVelocity(fwdAxis * dashSpeed);
					facingDirection = forward;
				}
				break;
			case NCL::CharacterController::backward:
				if (isGrounded()) {
					playerObject->GetRigidBody()->SetLinearVelocity(-fwdAxis * dashSpeed);
					facingDirection = backward;
				}
				break;
			default:
				break;
			}
		}
		if (dashTimeLeft <= 0) {
			playerObject->GetRigidBody()->SetLinearVelocity(playerObject->GetRigidBody()->GetLinearVelocity() / NCLVector3(5, 1, 5));
			isDashing = false;
		}
	}
}

void NCL::CharacterController::Dash()
{
	if (canDash) {
		if (dashCount > 0) {
			isDashing = true;
			dashTimeLeft = dashTime;
			dashCount--;
		}
		else {
			canDash = false;
			dashCount = 2;
		}
	}
}

bool NCL::CharacterController::StopMove() {
	if (pre_postition == playerObject->GetTransform().GetPosition())
		return true;
	else
		return false;
}
#endif _WIN64
