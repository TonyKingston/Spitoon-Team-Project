/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>
#include <string>
#include <math.h>
#include "foundation/PxVec3.h"

namespace NCL {
	namespace Maths {
		class NCLVector2;
		class NCLVector4;

		class NCLVector3 {
		public:
			union {
				struct {
					float x;
					float y;
					float z;
				};
				float array[3];
			};
		public:
			constexpr NCLVector3(void) : x(0.0f), y(0.0f), z(0.0f) {}

			constexpr NCLVector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

			NCLVector3(const NCLVector2& v2, float z = 0.0f);
			NCLVector3(const NCLVector4& v4);

			~NCLVector3(void) {}

			NCLVector3 Normalised() const {
				NCLVector3 temp(x, y, z);
				temp.Normalise();
				return temp;
			}

			void			Normalise() {
				float length = Length();

				if (length != 0.0f) {
					length = 1.0f / length;
					x = x * length;
					y = y * length;
					z = z * length;
				}
			}

			float	Length() const {
				return sqrt((x*x) + (y*y) + (z*z));
			}

			constexpr float	LengthSquared() const {
				return ((x*x) + (y*y) + (z*z));
			}

			constexpr float		GetMaxElement() const {
				float v = x;
				if (y > v) {
					v = y;
				}
				if (z > v) {
					v = z;
				}
				return v;
			}

			float		GetAbsMaxElement() const {
				float v = abs(x);
				if (abs(y) > v) {
					v = abs(y);
				}
				if (abs(z) > v) {
					v = abs(z);
				}
				return v;
			}


			inline physx::PxVec3 ToPxVec3() const {
				return physx::PxVec3(x, y, z);
			}

			static NCLVector3 Lerp(const NCLVector3& a, const NCLVector3& b, float t) {
				return (a * t) + (b * (1.0f - t));
			}

			static constexpr float	Dot(const NCLVector3 &a, const NCLVector3 &b) {
				return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
			}

			static NCLVector3	Cross(const NCLVector3 &a, const NCLVector3 &b) {
				return NCLVector3((a.y*b.z) - (a.z*b.y), (a.z*b.x) - (a.x*b.z), (a.x*b.y) - (a.y*b.x));
			}

			static float Angle(NCLVector3 from, NCLVector3 to) {
#ifdef _WIN64
				return std::acos(std::clamp(NCLVector3::Dot(from.Normalised(), to.Normalised()), -1.0f, 1.0f)) * 57.29578f;
#endif
				return 0.0f;
			}

			inline NCLVector3  operator+(const NCLVector3  &a) const {
				return NCLVector3(x + a.x, y + a.y, z + a.z);
			}

			inline NCLVector3  operator-(const NCLVector3  &a) const {
				return NCLVector3(x - a.x, y - a.y, z - a.z);
			}

			inline NCLVector3  operator-() const {
				return NCLVector3(-x, -y, -z);
			}

			inline NCLVector3  operator*(float a)	const {
				return NCLVector3(x * a, y * a, z * a);
			}

			inline NCLVector3  operator*(const NCLVector3  &a) const {
				return NCLVector3(x * a.x, y * a.y, z * a.z);
			}

			inline NCLVector3  operator/(const NCLVector3  &a) const {
				return NCLVector3(x / a.x, y / a.y, z / a.z);
			};

			inline NCLVector3  operator/(float v) const {
				return NCLVector3(x / v, y / v, z / v);
			};

			inline constexpr void operator+=(const NCLVector3  &a) {
				x += a.x;
				y += a.y;
				z += a.z;
			}

			inline void operator-=(const NCLVector3  &a) {
				x -= a.x;
				y -= a.y;
				z -= a.z;
			}


			inline void operator*=(const NCLVector3  &a) {
				x *= a.x;
				y *= a.y;
				z *= a.z;
			}

			inline void operator/=(const NCLVector3  &a) {
				x /= a.x;
				y /= a.y;
				z /= a.z;
			}

			inline void operator*=(float f) {
				x *= f;
				y *= f;
				z *= f;
			}

			inline void operator/=(float f) {
				x /= f;
				y /= f;
				z /= f;
			}

			inline float operator[](int i) const {
				return array[i];
			}

			inline float& operator[](int i) {
				return array[i];
			}

			inline std::string ToString()
			{
				return "(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")";
			};

			inline bool	operator==(const NCLVector3 &A)const { return (A.x == x && A.y == y && A.z == z) ? true : false; };
			inline bool	operator!=(const NCLVector3 &A)const { return (A.x == x && A.y == y && A.z == z) ? false : true; };

			inline friend std::ostream& operator<<(std::ostream& o, const NCLVector3& v) {
				o << "NCLVector3(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
				return o;
			}
		};
	}
}
