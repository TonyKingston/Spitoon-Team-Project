/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <iostream>

namespace NCL {
	namespace Maths {
		class NCLVector3;
		class NCLVector4;
		class NCLVector2 {
		public:
			union {
				struct {
					float x;
					float y;
				};
				float array[2];
			};

		public:
			constexpr NCLVector2(void) : x(0.0f), y(0.0f) {}

			constexpr NCLVector2(float xVal, float yVal) : x(xVal), y(yVal){}

			NCLVector2(const NCLVector3& v3);
			NCLVector2(const NCLVector4& v4);

			~NCLVector2(void) {}

			NCLVector2 Normalised() const {
				NCLVector2 temp(x, y);
				temp.Normalise();
				return temp;
			}

			void			Normalise() {
				float length = Length();

				if (length != 0.0f) {
					length = 1.0f / length;
					x = x * length;
					y = y * length;
				}
			}

			float	Length() const {
				return sqrt((x*x) + (y*y));
			}

			constexpr float	LengthSquared() const {
				return ((x*x) + (y*y));
			}

			constexpr float		GetMaxElement() const {
				float v = x;
				if (y > v) {
					v = y;
				}
				return v;
			}

			float		GetAbsMaxElement() const {
				float ax = abs(x);
				float ay = abs(y);

				if (ax > ay) {
					return ax;
				}
				return ay;
			}

			static constexpr float	Dot(const NCLVector2 &a, const NCLVector2 &b) {
				return (a.x*b.x) + (a.y*b.y);
			}

			inline NCLVector2  operator+(const NCLVector2  &a) const {
				return NCLVector2(x + a.x, y + a.y);
			}

			inline NCLVector2  operator-(const NCLVector2  &a) const {
				return NCLVector2(x - a.x, y - a.y);
			}

			inline NCLVector2  operator-() const {
				return NCLVector2(-x, -y);
			}

			inline NCLVector2  operator*(float a)	const {
				return NCLVector2(x * a, y * a);
			}

			inline NCLVector2  operator*(const NCLVector2  &a) const {
				return NCLVector2(x * a.x, y * a.y);
			}

			inline NCLVector2  operator/(const NCLVector2  &a) const {
				return NCLVector2(x / a.x, y / a.y);
			};

			inline NCLVector2  operator/(float v) const {
				return NCLVector2(x / v, y / v);
			};

			inline constexpr void operator+=(const NCLVector2  &a) {
				x += a.x;
				y += a.y;
			}

			inline void operator-=(const NCLVector2  &a) {
				x -= a.x;
				y -= a.y;
			}


			inline void operator*=(const NCLVector2  &a) {
				x *= a.x;
				y *= a.y;
			}

			inline void operator/=(const NCLVector2  &a) {
				x /= a.x;
				y /= a.y;
			}

			inline void operator*=(float f) {
				x *= f;
				y *= f;
			}

			inline void operator/=(float f) {
				x /= f;
				y /= f;
			}

			inline float operator[](int i) const {
				return array[i];
			}

			inline float& operator[](int i) {
				return array[i];
			}

			inline bool	operator==(const NCLVector2 &A)const { return (A.x == x && A.y == y) ? true : false; };
			inline bool	operator!=(const NCLVector2 &A)const { return (A.x == x && A.y == y) ? false : true; };

			inline friend std::ostream& operator<<(std::ostream& o, const NCLVector2& v) {
				o << "NCLVector2(" << v.x << "," << v.y << ")" << std::endl;
				return o;
			}
		};
	}
}
