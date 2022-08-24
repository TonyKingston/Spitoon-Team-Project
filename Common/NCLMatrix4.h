/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once

#include <iostream>
#include "foundation/PxMat44.h"

namespace NCL {
	namespace Maths {
		class NCLVector3;
		class NCLVector4;
		class NCLMatrix3;
		class Quaternion;

		class NCLMatrix4 {
		public:
			NCLMatrix4(void);
			NCLMatrix4(float elements[16]);
			NCLMatrix4(const NCLMatrix3& m3);
			NCLMatrix4(const Quaternion& quat);
			~NCLMatrix4(void);

			float	array[16];

			//Set all matrix values to zero
			void	ToZero();
			void    ToIdentity();

			physx::PxMat44 ToPxMat() const;

			//Gets the OpenGL position vector (floats 12,13, and 14)
			NCLVector3 GetPositionNCLVector() const;
			//Sets the OpenGL position vector (floats 12,13, and 14)
			void	SetPositionNCLVector(const NCLVector3 &in);

			//Gets the scale vector (floats 1,5, and 10)
			NCLVector3 GetDiagonal() const;
			//Sets the scale vector (floats 1,5, and 10)
			void	SetDiagonal(const NCLVector3& in);

			//Creates a rotation matrix that rotates by 'degrees' around the 'axis'
			//Analogous to glRotatef
			static NCLMatrix4 Rotation(float degrees, const NCLVector3& axis);

			//Creates a scaling matrix (puts the 'scale' vector down the diagonal)
			//Analogous to glScalef
			static NCLMatrix4 Scale(const NCLVector3& scale);

			//Creates a translation matrix (identity, with 'translation' vector at
			//floats 12, 13, and 14. Analogous to glTranslatef
			static NCLMatrix4 Translation(const NCLVector3& translation);

			//Creates a perspective matrix, with 'znear' and 'zfar' as the near and 
			//far planes, using 'aspect' and 'fov' as the aspect ratio and vertical
			//field of vision, respectively.
			static NCLMatrix4 Perspective(float znear, float zfar, float aspect, float fov);

			//Creates an orthographic matrix with 'znear' and 'zfar' as the near and 
			//far planes, and so on. Descriptive variable names are a good thing!
			static NCLMatrix4 Orthographic(float znear, float zfar, float right, float left, float top, float bottom);

			//Builds a view matrix suitable for sending straight to the vertex shader.
			//Puts the camera at 'from', with 'lookingAt' centered on the screen, with
			//'up' as the...up axis (pointing towards the top of the screen)
			static NCLMatrix4 BuildViewMatrix(const NCLVector3& from, const NCLVector3& lookingAt, const NCLVector3& up);

			void    Invert();
			NCLMatrix4 Inverse() const;


			NCLVector4 GetRow(unsigned int row) const;
			NCLVector4 GetColumn(unsigned int column) const;

			NCLMatrix4 Lerp(const NCLMatrix4& from, const NCLMatrix4& to, float p);

			//Multiplies 'this' matrix by matrix 'a'. Performs the multiplication in 'OpenGL' order (ie, backwards)
			inline NCLMatrix4 operator*(const NCLMatrix4& a) const {
				NCLMatrix4 out;
				//Students! You should be able to think up a really easy way of speeding this up...
				for (unsigned int r = 0; r < 4; ++r) {
					for (unsigned int c = 0; c < 4; ++c) {
						out.array[c + (r * 4)] = 0.0f;
						for (unsigned int i = 0; i < 4; ++i) {
							out.array[c + (r * 4)] += this->array[c + (i * 4)] * a.array[(r * 4) + i];
						}
					}
				}
				return out;
			}

			NCLVector3 operator*(const NCLVector3& v) const;
			NCLVector4 operator*(const NCLVector4& v) const;

			//Handy string output for the matrix. Can get a bit messy, but better than nothing!
			inline friend std::ostream& operator<<(std::ostream& o, const NCLMatrix4& m) {
				o << "Mat4(";
				o << "\t" << m.array[0] << "," << m.array[1] << "," << m.array[2] << "," << m.array[3] << std::endl;
				o << "\t\t" << m.array[4] << "," << m.array[5] << "," << m.array[6] << "," << m.array[7] << std::endl;
				o << "\t\t" << m.array[8] << "," << m.array[9] << "," << m.array[10] << "," << m.array[11] << std::endl;
				o << "\t\t" << m.array[12] << "," << m.array[13] << "," << m.array[14] << "," << m.array[15] << " )" << std::endl;
				return o;
			}
		};
	}
}
