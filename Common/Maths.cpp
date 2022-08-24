/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Maths.h"
#include "../Common/NCLVector2.h"
#include "../Common/NCLVector3.h"

namespace NCL {
	namespace Maths {
		void ScreenBoxOfTri(const NCLVector3& v0, const NCLVector3& v1, const NCLVector3& v2, NCLVector2& topLeft, NCLVector2& bottomRight) {
			topLeft.x = std::min(v0.x, std::min(v1.x, v2.x));
			topLeft.y = std::min(v0.y, std::min(v1.y, v2.y));

			bottomRight.x = std::max(v0.x, std::max(v1.x, v2.x));
			bottomRight.y = std::max(v0.y, std::max(v1.y, v2.y));
		}

		int ScreenAreaOfTri(const NCLVector3 &a, const NCLVector3 &b, const NCLVector3 & c) {
			int area =(int) (((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y)));
			return (area >> 1);
		}

		float FloatAreaOfTri(const NCLVector3 &a, const NCLVector3 &b, const NCLVector3 & c) {
			float area = ((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y));
			return (area * 0.5f);
		}

		float CrossAreaOfTri(const NCLVector3 &a, const NCLVector3 &b, const NCLVector3 & c) {
			NCLVector3 area = NCLVector3::Cross(a - b, a - c);
			return area.Length() * 0.5f;
		}
	

		NCLVector3 Clamp(const NCLVector3& a, const NCLVector3&mins, const NCLVector3& maxs) {
			return NCLVector3(
				Clamp(a.x, mins.x, maxs.x),
				Clamp(a.y, mins.y, maxs.y),
				Clamp(a.z, mins.z, maxs.z)
			);
		}
	}
}