/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <iostream>
#include "foundation/PxVec4.h"

namespace NCL {
	namespace Maths {
		class NCLVector3;
		class NCLVector2;

		class NCLVector4 {

		public:
			union {
				struct {
					float x;
					float y;
					float z;
					float w;
				};
				float array[4];
			};

		public:
			constexpr NCLVector4(void) : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

			constexpr NCLVector4(float xVal, float yVal, float zVal, float wVal) : x(xVal), y(yVal), z(zVal), w(wVal) {}

			NCLVector4(const NCLVector3& v3, float w = 0.0f);
			NCLVector4(const NCLVector2& v2, float z = 0.0f, float w = 0.0f);

			~NCLVector4(void) {}

			inline physx::PxVec4 ToPxVec4() const {
				return physx::PxVec4(x, y, z, w);
			}

			inline static NCLVector4 ConvertDecimalColour(int colour) {
				int r = colour >> 16 & 0xff;
				int g = colour >> 8 & 0xff;
				int b = colour & 0xff;
				return NCLVector4(r, g, b, 1.0f);
			}

			NCLVector4 Normalised() const {
				NCLVector4 temp(x, y, z, w);
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
					w = w * length;
				}
			}

			float	Length() const {
				return sqrt((x*x) + (y*y) + (z*z) + (w * w));
			}

			constexpr float	LengthSquared() const {
				return ((x*x) + (y*y) + (z*z) + (w * w));
			}

			constexpr float		GetMaxElement() const {
				float v = x;
				if (y > v) {
					v = y;
				}
				if (z > v) {
					v = z;
				}
				if (w > v) {
					v = w;
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
				if (abs(w) > v) {
					v = abs(w);
				}
				return v;
			}

			static float	Dot(const NCLVector4 &a, const NCLVector4 &b) {
				return (a.x*b.x) + (a.y*b.y) + (a.z*b.z) + (a.w*b.w);
			}

			inline NCLVector4  operator+(const NCLVector4  &a) const {
				return NCLVector4(x + a.x, y + a.y, z + a.z, w + a.w);
			}

			inline NCLVector4  operator-(const NCLVector4  &a) const {
				return NCLVector4(x - a.x, y - a.y, z - a.z, w - a.w);
			}

			inline NCLVector4  operator-() const {
				return NCLVector4(-x, -y, -z, -w);
			}

			inline NCLVector4  operator*(float a)	const {
				return NCLVector4(x * a, y * a, z * a, w * a);
			}

			inline NCLVector4  operator*(const NCLVector4  &a) const {
				return NCLVector4(x * a.x, y * a.y, z * a.z, w * a.w);
			}

			inline NCLVector4  operator/(const NCLVector4  &a) const {
				return NCLVector4(x / a.x, y / a.y, z / a.z, w / a.w);
			};

			inline NCLVector4  operator/(float v) const {
				return NCLVector4(x / v, y / v, z / v, w / v);
			};

			inline constexpr void operator+=(const NCLVector4  &a) {
				x += a.x;
				y += a.y;
				z += a.z;
				w += a.w;
			}

			inline void operator-=(const NCLVector4  &a) {
				x -= a.x;
				y -= a.y;
				z -= a.z;
				w -= a.w;
			}


			inline void operator*=(const NCLVector4  &a) {
				x *= a.x;
				y *= a.y;
				z *= a.z;
				w *= a.w;
			}

			inline void operator/=(const NCLVector4  &a) {
				x /= a.x;
				y /= a.y;
				z /= a.z;
				w /= a.w;
			}

			inline void operator*=(float f) {
				x *= f;
				y *= f;
				z *= f;
				w *= f;
			}

			inline void operator/=(float f) {
				x /= f;
				y /= f;
				z /= f;
				w /= f;
			}

			inline float operator[](int i) const {
				return array[i];
			}

			inline float& operator[](int i) {
				return array[i];
			}

			inline bool	operator==(const NCLVector4 &A)const { return (A.x == x && A.y == y && A.z == z && A.w == w) ? true : false; };
			inline bool	operator!=(const NCLVector4 &A)const { return (A.x == x && A.y == y && A.z == z && A.w == w) ? false : true; };

			inline friend std::ostream& operator<<(std::ostream& o, const NCLVector4& v) {
				o << "NCLVector4(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")" << std::endl;
				return o;
			}
		};

		static NCLVector4 COLOUR_BLACK(0, 0, 0, 1.0f);
		static NCLVector4 COLOUR_WHITE(1, 1, 1, 1.0f);
		static NCLVector4 HOMOGENEOUS(0, 0, 0, 1.0f);
	}
}