/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <algorithm>

namespace NCL {
	namespace Maths {
		class NCLVector2;
		class NCLVector3;

		//It's pi(ish)...
		static const float		PI = 3.14159265358979323846f;

		//It's pi...divided by 360.0f!
		static const float		PI_OVER_360 = PI / 360.0f;

		//Radians to degrees
		inline float RadiansToDegrees(float rads) {
			return rads * 180.0f / PI;
		};

		//Degrees to radians
		inline float DegreesToRadians(float degs) {
			return degs * PI / 180.0f;
		};

		template<class T>
		inline T Clamp(T value, T min, T max) {
			if (value < min) {
				return min;
			}
			if (value > max) {
				return max;
			}
			return value;
		}

		NCLVector3 Clamp(const NCLVector3& a, const NCLVector3&mins, const NCLVector3& maxs);

		template<class T>
		inline T Lerp(const T& a, const T&b, float by) {
			return (a * (1.0f - by) + b*by);
		}

		void ScreenBoxOfTri(const NCLVector3& v0, const NCLVector3& v1, const NCLVector3& v2, NCLVector2& topLeft, NCLVector2& bottomRight);

		int ScreenAreaOfTri(const NCLVector3 &a, const NCLVector3 &b, const NCLVector3 & c);
		float FloatAreaOfTri(const NCLVector3 &a, const NCLVector3 &b, const NCLVector3 & c);

		float CrossAreaOfTri(const NCLVector3 &a, const NCLVector3 &b, const NCLVector3 & c);
	}
}