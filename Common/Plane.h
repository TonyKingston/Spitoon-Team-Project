/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "NCLVector3.h"
namespace NCL {
	namespace Maths {
		class Plane {
		public:
			Plane(void);
			Plane(const NCLVector3 &normal, float distance, bool normalise = false);

			~Plane(void) {};

			//Sets the planes normal, which should be UNIT LENGTH!!!
			void	SetNormal(const NCLVector3 &normal) { this->normal = normal; }
			//Gets the planes normal.
			NCLVector3 GetNormal() const { return normal; }
			//Sets the planes distance from the origin
			void	SetDistance(float dist) { distance = dist; }
			//Gets the planes distance from the origin
			float	GetDistance() const { return distance; }
			//Performs a simple sphere / plane test
			bool SphereInPlane(const NCLVector3 &position, float radius) const;
			//Performs a simple sphere / point test
			bool PointInPlane(const NCLVector3 &position) const;

			float	DistanceFromPlane(const NCLVector3 &in) const;

			NCLVector3 GetPointOnPlane() const {
				return normal * -distance;
			}

			NCLVector3 ProjectPointOntoPlane(const NCLVector3 &point) const;


			static Plane PlaneFromTri(const NCLVector3 &v0, const NCLVector3 &v1, const NCLVector3 &v2);

		protected:
			//Unit-length plane normal
			NCLVector3 normal;
			//Distance from origin
			float	distance;
		};
	}
}