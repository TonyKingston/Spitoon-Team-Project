#pragma once
#include "foundation/PxErrorCallback.h"
#include <iostream>
#include <string>
using namespace physx;
using std::cout;
using std::endl;
using std::to_string;

namespace NCL {
    namespace CSC8503 {
        
        class BasicErrorCallback : public PxErrorCallback {
        public:
            void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) {
                cout << "PhysX Error Report" << endl;
                cout << "PhysX Error Code: " + to_string(code) << endl;
                cout << "PhysX Message: " << message << endl;
                cout << "File: " << file << " line: " + line << endl;
            }
        };
    }
}

