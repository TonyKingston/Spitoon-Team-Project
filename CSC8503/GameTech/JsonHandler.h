#pragma once

#include "../CSC8503Common/json.h"
#include "../CSC8503Common/json-forwards.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include "../GameTech/CourseworkGame.h"

using std::string;
using NCL::Maths::NCLVector3;

namespace NCL{
	namespace CSC8503 {
		class CourseworkGame;

		class JsonHandler
		{
		public:
			
			JsonHandler(){}
			~JsonHandler() {}

			static void ReadFile(const string& filename, CourseworkGame* game);
			static void WriteFile(const string& filename, const NCLVector3& position, const NCLVector3& orientation, const NCLVector3& scale,
				const string& wMesh, const string& wMaterial,
				const string& wType, const NCLVector3& wBarycentre, const NCLVector3& wHalfExtents, float wRadius, float wHalfHeight,
				bool wIsStatic, bool wIsKinematic, float wSFriction, float wDFriction, float wRestitution);
		private:
			static NCLVector3 JsonToVec3(Json::Value vec) {
				return NCLVector3(vec["x"].asFloat(), vec["y"].asFloat(), vec["z"].asFloat());
			}

			static Quaternion JsonToQuat(Json::Value quat) {
				return Quaternion(quat["x"].asFloat(), quat["y"].asFloat(), quat["z"].asFloat(), quat["w"].asFloat());
			}
		};
	}
}

