#ifdef _WIN64
#pragma once
#include "NCLMatrix4.h"
#include "NCLVector3.h"

namespace NCL {
	using namespace NCL::Maths;
	enum class CameraType {
		Orthographic,
		Perspective
	};

	class Camera {
	public:
		Camera(void) {
			left	= 0;
			right	= 0;
			top		= 0;
			bottom	= 0;

			pitch		= 0.0f;
			yaw			= 0.0f;

			fov			= 45.0f;
			nearPlane	= 1.0f;
			farPlane	= 100.0f;

			camType		= CameraType::Perspective;
		};

		Camera(float pitch, float yaw, const NCLVector3& position) : Camera() {
			this->pitch		= pitch;
			this->yaw		= yaw;
			this->position	= position;

			this->fov		= 45.0f;
			this->nearPlane = 1.0f;
			this->farPlane	= 100.0f;

			this->camType	= CameraType::Perspective;
		}

		~Camera(void) {};

		void UpdateCamera(float dt);
		void UpdateCamera_DEBUG(float dt);
		void UpdateCamera_TPS(float dt, const NCLVector3& obj_position, int playerNum, bool isSplitScreen = false);

		float GetFieldOfVision() const {
			return fov;
		}

		float GetNearPlane() const {
			return nearPlane;
		}

		float GetFarPlane() const {
			return farPlane;
		}

		void SetNearPlane(float val) {
			nearPlane = val;
		}
		
		void SetFarPlane(float val) {
			farPlane = val;
		}

		//Builds a view matrix for the current camera variables, suitable for sending straight
		//to a vertex shader (i.e it's already an 'inverse camera matrix').
		NCLMatrix4 BuildViewMatrix() const;

		NCLMatrix4 BuildProjectionMatrix(float currentAspect = 1.0f) const;

		//Gets position in world space
		NCLVector3 GetPosition() const { return position; }
		//Sets position in world space
		void	SetPosition(const NCLVector3& val) { position = val; }

		//Gets yaw, in degrees
		float	GetYaw()   const { return yaw; }
		//Sets yaw, in degrees
		void	SetYaw(float y) { yaw = y; }

		//Gets pitch, in degrees
		float	GetPitch() const { return pitch; }
		//Sets pitch, in degrees
		void	SetPitch(float p) { pitch = p; }
		
		void InitView(const NCLVector3& obj_position);
		void LookAt(const NCLVector3 &dir);

		static Camera BuildPerspectiveCamera(const NCLVector3& pos, float pitch, float yaw, float fov, float near, float far);
		static Camera BuildOrthoCamera(const NCLVector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float near, float far);
	protected:
		CameraType camType;

		float	nearPlane;
		float	farPlane;
		float	left;
		float	right;
		float	top;
		float	bottom;

		float	fov;
		float	yaw;
		float	pitch;
		NCLVector3 position;

		NCLVector3 offset = NCLVector3(0, 0, 20);
		NCLVector3 offsetForFullScreen = NCLVector3(0, 0, 20);
	};
}

#endif _WIN64