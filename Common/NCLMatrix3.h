/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <assert.h>
#include <algorithm>
#include <iostream>

namespace NCL {
	namespace Maths {
		class NCLMatrix2;
		class NCLMatrix4;
		class NCLVector3;
		class Quaternion;

		class NCLMatrix3
		{
		public:
			NCLMatrix3(void);
			NCLMatrix3(float elements[9]);
			NCLMatrix3(const NCLMatrix2 &m4);
			NCLMatrix3(const NCLMatrix4 &m4);
			NCLMatrix3(const Quaternion& quat);

			~NCLMatrix3(void);

			//Set all matrix values to zero
			void	ToZero();

			NCLVector3 GetRow(unsigned int row) const;
			void	SetRow(unsigned int row, const NCLVector3 &val);

			NCLVector3 GetColumn(unsigned int column) const;
			void	SetColumn(unsigned int column, const NCLVector3 &val);

			NCLVector3 GetDiagonal() const;
			void	SetDiagonal(const NCLVector3 &in);

			NCLVector3 ToEuler() const;

			inline NCLMatrix3 Absolute() const {
				NCLMatrix3 m;

				for (int i = 0; i < 9; ++i) {
					m.array[i] = std::abs(array[i]);
				}

				return m;
			}

			inline NCLMatrix3 Transposed() const {
				NCLMatrix3 temp = *this;
				temp.Transpose();
				return temp;
			}

			inline void Transpose() {
				float tempValues[3];

				tempValues[0] = array[3];
				tempValues[1] = array[6];
				tempValues[2] = array[7];

				array[3] = array[1];
				array[6] = array[2];
				array[7] = array[5];

				array[1] = tempValues[0];
				array[2] = tempValues[1];
				array[5] = tempValues[2];
			}

			NCLVector3 operator*(const NCLVector3 &v) const;

			inline NCLMatrix3 operator*(const NCLMatrix3 &a) const {
				NCLMatrix3 out;
				//Students! You should be able to think up a really easy way of speeding this up...
				for (unsigned int r = 0; r < 3; ++r) {
					for (unsigned int c = 0; c < 3; ++c) {
						out.array[c + (r * 3)] = 0.0f;
						for (unsigned int i = 0; i < 3; ++i) {
							out.array[c + (r * 3)] += this->array[c + (i * 3)] * a.array[(r * 3) + i];
						}
					}
				}
				return out;
			}

			//Creates a rotation matrix that rotates by 'degrees' around the 'axis'
			//Analogous to glRotatef
			static NCLMatrix3 Rotation(float degrees, const NCLVector3 &axis);

			//Creates a scaling matrix (puts the 'scale' vector down the diagonal)
			//Analogous to glScalef
			static NCLMatrix3 Scale(const NCLVector3 &scale);

			static NCLMatrix3 FromEuler(const NCLVector3 &euler);
		public:
			float array[9];
		};

		//Handy string output for the matrix. Can get a bit messy, but better than nothing!
		inline std::ostream& operator<<(std::ostream& o, const NCLMatrix3& m) {
			o << m.array[0] << "," << m.array[1] << "," << m.array[2] << std::endl;
			o << m.array[3] << "," << m.array[4] << "," << m.array[5] << std::endl;
			o << m.array[6] << "," << m.array[7] << "," << m.array[8];
			return o;
		}

		inline std::istream& operator >> (std::istream& i, NCLMatrix3& m) {
			char ignore;
			i >> std::skipws;
			i >> m.array[0] >> ignore >> m.array[1] >> ignore >> m.array[2];
			i >> m.array[3] >> ignore >> m.array[4] >> ignore >> m.array[5];
			i >> m.array[6] >> ignore >> m.array[7] >> ignore >> m.array[8];

			return i;
		}
	}
}