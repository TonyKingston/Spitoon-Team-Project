#include "JsonHandler.h"
#include "..//..//Common/Assets.h"
#include <iostream>
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/Physics/Types.h"

using namespace NCL::CSC8503;
#ifdef _WIN64
void JsonHandler::ReadFile(const string& filename, CourseworkGame* game) {
	Json::CharReaderBuilder reader;
	Json::Value root;
	reader["collectComments"] = false;
	std::string errs;
	std::ifstream ifs(Assets::JSONADIR + filename);
	if (!Json::parseFromStream(reader, ifs, &root, &errs))
	{
		std::cout << "fail to open file!" << std::endl;
	}
	else {
		for (Json::Value::ArrayIndex i = 0; i != root.size(); i++) {

			string mesh = "";
			string material = "";
			//collision properties
			string type = "";
			NCLVector3 barycentre = NCLVector3();
			NCLVector3 halfExtents = NCLVector3();
			float radius = 0;
			float halfHeight = 0;
			//physics properties
			RigidBodyParams params;

			//trans
			   //position
			NCLVector3 position = JsonToVec3(root[i]["trans"]["position"]);

			//orientation
			NCLVector3 orientation = JsonToVec3(root[i]["trans"]["orientation"]);

			//scale
			NCLVector3 scale = JsonToVec3(root[i]["trans"]["scale"]);


			//rend
			   //mesh
			mesh = root[i]["rend"]["mesh"].asString();
			material = root[i]["rend"]["material"].asString();

			//collision
			   //type
			type = root[i]["collision"]["type"].asString();
			barycentre = JsonToVec3(root[i]["collision"]["barycentre"]);
			if (type == "box") {
				halfExtents = scale / 2;
			}
			else if (type == "sphere") {
				radius = root[i]["collision"]["radius"].asFloat();
			}
			else if (type == "capsule") {
				radius = root[i]["collision"]["radius"].asFloat();
				halfHeight = root[i]["collision"]["halfHeight"].asFloat();
			}

			//physics
			   //isStatic
			params.isStatic = root[i]["physics"]["isStatic"].asBool();
			//isKinematic
			params.isKinematic = root[i]["physics"]["isKinematic"].asBool();
			//sFriction
			params.sFriction = root[i]["physics"]["sFriction"].asFloat();
			//dFriction
			params.dFriction = root[i]["physics"]["dFriction"].asFloat();
			//restitution
			params.restitution = root[i]["physics"]["restitution"].asFloat();


			//params.sFriction = 0.0;
			////dFriction
			//params.dFriction = 0.0;
			////restitution
			//params.restitution = 0.0;

			GameObject* gameObject = new GameObject();
			gameObject->GetTransform()
				.SetPosition(NCLVector3(position.x + 3, position.y, -position.z + 15))
				.SetOrientation(Quaternion::EulerAnglesToQuaternion(orientation.x, orientation.y,orientation.z))
				.SetScale(scale);
			Quaternion rotate = Quaternion::EulerAnglesToQuaternion(0, 0, -180);
			gameObject->GetTransform().SetOrientation(gameObject->GetTransform().GetOrientation());

			RigidBody* body = new RigidBody(&gameObject->GetTransform(), params);
			if (type == "sphere") {
				body->AddSphereShape(radius);
			}
			else if (type == "box") {
				body->AddBoxShape(halfExtents);
			}
			else if (type == "capsule") {
				body->AddCapsuleShape(radius, halfHeight);
			}
			else if (type == "plane") {
				body->AddPlaneShape();
			}
			else if (type == "mesh") {
				body->AddConvexMesh(game->GetResourceManager()->LoadMesh(mesh));
			}

			if (params.isKinematic) body->SetKinematic(true);
			body->SetCollisionFilters(CollisionLayers::Default, CollisionLayers::Default | CollisionLayers::Paint);
			gameObject->SetPhysicsObject(body);
			vector<TextureBase*> textures;
#ifdef _WIN64
			if (!game->GetResourceManager()->LoadMaterial(material, textures)) {
				textures.emplace_back(game->GetResourceManager()->LoadTexture("checkerboard.png"));
			}
#endif _WIN64

			gameObject->SetRenderObject(new RenderObject(&gameObject->GetTransform(), game->GetResourceManager()->LoadMesh(mesh),
				textures, game->GetResourceManager()->LoadShader("GameTechVert.glsl", "bufferFragment.glsl")));

			game->GetPhysics()->AddRigidBody(body);
			game->GetWorld()->AddGameObject(gameObject);
		}
	}
}

void JsonHandler::WriteFile(const string& filename, const NCLVector3& position, const NCLVector3& orientation, const NCLVector3& scale,
	const string& wMesh, const string& wMaterial,
	const string& wType, const NCLVector3& wBarycentre, const NCLVector3& wHalfExtents, float wRadius, float wHalfHeight,
	bool wIsStatic, bool wIsKinematic, float wSFriction, float wDFriction, float wRestitution){
	Json::Value root;

	//trans properties
	Json::Value trans, _position, _orientation, _scale;
	_position["x"] = position.x;
	_position["y"] = position.y;
	_position["z"] = position.z;
	trans["position"] = _position;

	_orientation["x"] = orientation.x;
	_orientation["y"] = orientation.y;
	_orientation["z"] = orientation.z;
	trans["orientation"] = _orientation;

	_scale["x"] = scale.x;
	_scale["y"] = scale.y;
	_scale["z"] = scale.z;
	trans["scale"] = _scale;

	root["trans"] = trans;


	//rend properties
	Json::Value rend;
	rend["mesh"] = wMesh;
	rend["material"] = wMaterial;
	root["rend"] = rend;

	//collision properties
	Json::Value collision, _barycentre, _halfExtents;
	collision["type"] = wType;
	collision["radius"] = wRadius;
	collision["halfHeight"] = wHalfHeight;

	_barycentre["x"] = wBarycentre.x;
	_barycentre["y"] = wBarycentre.y;
	_barycentre["z"] = wBarycentre.z;
	collision["barycentre"] = _barycentre;

	_halfExtents["x"] = wHalfExtents.x;
	_halfExtents["y"] = wHalfExtents.y;
	_halfExtents["z"] = wHalfExtents.z;
	collision["halfExtents"] = _halfExtents;


	root["collision"] = collision;


	//physics properties
	Json::Value physics;
	physics["isStatic"] = wIsStatic;
	physics["isKinematic"] = wIsKinematic;
	physics["sFriction"] = wSFriction;
	physics["dFriction"] = wDFriction;
	physics["restitution"] = wRestitution;

	root["physics"] = physics;

	////write other properties
	//root["radius"] = myRadius;
	//root["HalfHeight"] = myHalfHeight;
	//root["type"] = myType;
	//root["inverseMass"] = std::to_string(inverseMass);


	Json::StreamWriterBuilder wBuilder;
	std::unique_ptr<Json::StreamWriter> writer(wBuilder.newStreamWriter());
	std::ofstream ofs;
	ofs.open(Assets::JSONADIR + filename);
	if (!ofs.is_open())
		std::cout << "fail to open file!" << std::endl;
	writer->write(root, &ofs);
	ofs.close();
}
#endif _WIN64