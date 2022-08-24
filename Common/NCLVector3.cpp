/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "NCLVector4.h"
#include "NCLVector3.h"
#include "NCLVector2.h"
using namespace NCL;
using namespace Maths;


NCLVector3::NCLVector3(const NCLVector2& v2, float newZ) : x(v2.x), y(v2.y), z(newZ) {
}

NCLVector3::NCLVector3(const NCLVector4& v4) : x(v4.x), y(v4.y), z(v4.z) {
}