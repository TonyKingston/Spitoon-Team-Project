#ifdef _WIN64
#include "Camera.h"
#include "Window.h"
#include "Win32Window.h"
#include <algorithm>
#include <vector>
#include <cmath>

#include "../../Common/Quaternion.h"
#include <SFML/Window.hpp>

#define RADIUS_TO_DEGREE 180.0f/3.141592625f
using namespace NCL;
using namespace Win32Code;

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float dt) {
	//Update the mouse by how much
	pitch	-= (Window::GetMouse()->GetRelativePosition().y);
	yaw		-= (Window::GetMouse()->GetRelativePosition().x);

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw <0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	float frameSpeed = 100 * dt;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		position += NCLMatrix4::Rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(0, 0, -1) * frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		position -= NCLMatrix4::Rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(0, 0, -1) * frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		position += NCLMatrix4::Rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(-1, 0, 0) * frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		position -= NCLMatrix4::Rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(-1, 0, 0) * frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
		position.y += frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
		position.y -= frameSpeed;
	}
}

void Camera::UpdateCamera_DEBUG(float dt) {
	if (Window::GetMouse()->ButtonHeld(MouseButtons::RIGHT)) {
		//Update the rotation
		pitch -= (Window::GetMouse()->GetRelativePosition().y) * 2;
		yaw -= (Window::GetMouse()->GetRelativePosition().x) * 2;

		//Bounds check the pitch, to be between straight up and straight down ;)
		pitch = std::min(pitch, 90.0f);
		pitch = std::max(pitch, -90.0f);

		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}
	}

	float frameSpeed = 100 * dt;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		position += NCLMatrix4::Rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(0, 0, -1) * frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		position -= NCLMatrix4::Rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(0, 0, -1) * frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		position += NCLMatrix4::Rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(-1, 0, 0) * frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		position -= NCLMatrix4::Rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(-1, 0, 0) * frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
		position.y += frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
		position.y -= frameSpeed;
	}

}

std::vector<float> test;
void Camera::UpdateCamera_TPS(float dt, const NCLVector3& obj_position, int playerNum, bool isSplitScreen) {
	sf::Joystick::update();
	float z = sf::Joystick::getAxisPosition(0, sf::Joystick::Z);
	float r = sf::Joystick::getAxisPosition(0, sf::Joystick::R);

	float frame_speed = 50 * dt;

	float temp_pitch = pitch;
	float temp_yaw = yaw;
	NCLVector3 position_offset = NCLVector3();

	NCLVector3 temp_offset;

	float mouse_x = Window::GetMouse()->GetRelativePosition().x;
	float mouse_y = Window::GetMouse()->GetRelativePosition().y;

	if (isSplitScreen) {
		switch (playerNum)
		{
		case 1:
			temp_pitch -= mouse_y * frame_speed;
			temp_yaw -= mouse_x * frame_speed;
			/*position_offset = NCLVector3(4, 5, 0);*/
			break;
		case 2:
			if (z > 50 || z < -50) {
				temp_yaw -= (z / 50);
			}

			if (r > 50 || r < -50) {
				temp_pitch -= (r / 50);
			}
		/*	position_offset = NCLVector3(-5, 3, 0);*/
			break;
		default:
			break;
		}
	}
	else {
		temp_yaw -= mouse_x * frame_speed;
		if (z > 50 || z < -50) {
			temp_yaw -= (z / 50);
		}

		temp_pitch -= mouse_y * frame_speed;
		if (r > 50 || r < -50) {
			temp_pitch -= (r / 50);
		}
	}

	Quaternion quat = Quaternion::EulerAnglesToQuaternion(temp_pitch, temp_yaw, 0);


	temp_offset = quat * offset;
	position = (obj_position + temp_offset + NCLVector3(0, 3, 0));



	LookAt(-temp_offset);//direction from camera to obj
}

void Camera::LookAt(const NCLVector3& dir) {
	yaw = std::atan2f(dir.x, dir.z) * RADIUS_TO_DEGREE + 180.0f;
	pitch = std::atanf(dir.y / std::sqrtf(dir.x * dir.x + dir.z * dir.z)) * RADIUS_TO_DEGREE;

	pitch = std::min(pitch, 10.0f);
	pitch = std::max(pitch, -80.0f);
}


void Camera::InitView(const NCLVector3& obj_position) {
	position = obj_position + NCLVector3(0, 0, 70);
	NCLMatrix4 temp = NCLMatrix4::BuildViewMatrix(position, obj_position, NCLVector3(0.0f, 1.0f, 0.0f));

	NCLMatrix4 modelMat = temp.Inverse();

	Quaternion q(modelMat);
	NCLVector3 angles = q.ToEuler();

	pitch = angles.x;
	yaw = angles.y;
}


/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
NCLMatrix4 Camera::BuildViewMatrix() const {
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	 NCLMatrix4::Rotation(-pitch, NCLVector3(1, 0, 0)) *
		NCLMatrix4::Rotation(-yaw, NCLVector3(0, 1, 0)) *
		NCLMatrix4::Translation(-position);
};

NCLMatrix4 Camera::BuildProjectionMatrix(float currentAspect) const {
	if (camType == CameraType::Orthographic) {
		return NCLMatrix4::Orthographic(nearPlane, farPlane, right, left, top, bottom);
	}
	//else if (camType == CameraType::Perspective) {
		return NCLMatrix4::Perspective(nearPlane, farPlane, currentAspect, fov);
	//}
}

Camera Camera::BuildPerspectiveCamera(const NCLVector3& pos, float pitch, float yaw, float fov, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Perspective;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane  = farPlane;

	c.fov		= fov;

	return c;
}
Camera Camera::BuildOrthoCamera(const NCLVector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Orthographic;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane	= farPlane;

	c.left		= left;
	c.right		= right;
	c.top		= top;
	c.bottom	= bottom;

	return c;
}
#endif
