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

NCLVector2::NCLVector2(const NCLVector3& v3) : x(v3.x), y(v3.y){
}

NCLVector2::NCLVector2(const NCLVector4& v4) : x(v4.x), y(v4.y) {
}