#include "Network_Code.h"
#include "Client_Net_Common.h"
#include "CourseworkGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/NavigationMesh.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#ifdef _WIN64
#include "GamePushdownState_UI.h"
#include <SFML/Window.hpp>
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/Win32Window.h"
#include "../CSC8503Common/NavMeshRenderer.h"
#include "../../UI/GUI.h"
#endif
#include "JsonHandler.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include "GamePushdownState.h"
//#include "CharacterController.cpp"



using namespace NCL;
using namespace CSC8503;
#ifdef _x64

CourseworkGame::CourseworkGame(Window* w) {
	onUserCreate();
	audio::Init();
	world = new GameWorld();
	meshMap = new NavigationMesh("Final01.navmesh");
	meshMap02 = new NavigationMesh("Final02.navmesh");
	testPaint = new vector<GameObject*>();
	testPaint02 = new vector<GameObject*>();
	//renderer->GetNavMesh(meshMap);
	gameUI = new D_GUI();
	resourceManager = new OGLResourceManager();
	physics = new PxSystem(*world);
	renderer = new GameTechRenderer(*world, gameUI, resourceManager);
	config = new SystemConfig();
	RigidBody::InitPhysics(physics);
	RenderObject::InitManager(resourceManager);
	window = w;
	reservoir.emplace_back(NCLVector3(0, 0, 0));
	forceMagnitude = 10.0f;
	Debug::SetRenderer(renderer);
	Window::GetWindow()->ShowOSPointer(true);
	Window::GetWindow()->LockMouseToWindow(true);

	config->LoadConfig("config.json");
	audio::LoadAudio();
	audio::PlayBGM();
	audio::PauseBGM(true);

	InitialiseAssets();
	InitialiseAnimations();
}
#endif

#ifdef _x64
bool CourseworkGame::onUserCreate() {

	//if (Connect("127.0.0.1", 60000)) {
	if (Connect("100.118.128.61", 60000)) {
		return true;
	}

	return false;
}
bool CourseworkGame::onUserUpdate() {
	if (renderer->inSplitScreen)
		return true;
	// Check for incoming network messages
	if (IsConnected()) {
		while (!Incoming().empty()) {
			auto msg = Incoming().pop_front().msg;

			switch (msg.header.id) {
			case(GameMsg::Client_Accepted):
			{
				//std::cout << "Server accepted client - you're in!\n";
				NCL::net::message<GameMsg> msg;
				msg.header.id = GameMsg::Client_RegisterWithServer;
				NCLVector3 spawnpoint = (numplayers == 1) ? (meshMap->allTris[5].centroid + NCLVector3(100, 1.0f, -105)) : (meshMap->allTris[8].centroid + NCLVector3(-10, 1.0f, -105));
				descPlayer.posX[nPlayerID] = spawnpoint.x;
				descPlayer.posY[nPlayerID] = spawnpoint.y;
				descPlayer.posZ[nPlayerID] = spawnpoint.z;
				msg << descPlayer;
				Send(msg);
				break;
			}

			case(GameMsg::Client_AssignID):
			{
				// Server is assigning us OUR id
				msg >> nPlayerID;
				descPlayer.nUniqueID = nPlayerID;
				//std::cout << "Assigned Client ID = " << nPlayerID << "\n";
				break;
			}

			case(GameMsg::Game_AddPlayer):
			{
				sPlayerDescription desc;
				msg >> desc;
				mapObjects.insert_or_assign(desc.nUniqueID, desc);
				LoadPlayer();
				if (desc.nUniqueID == nPlayerID) {
					// Now we exist in game world
					bWaitingForConnection = false;
				}
				break;
			}

			case(GameMsg::Game_RemovePlayer):
			{
				uint32_t nRemovalID = 0;
				msg >> nRemovalID;
				mapObjects.erase(nRemovalID);
				break;
			}

			case(GameMsg::Game_UpdatePlayer):
			{
				sPlayerDescription desc;
				msg >> desc;


				if (players.size() == 2)
					players[1 - nPlayerID]->GetTransform().SetPosition(NCLVector3(desc.posX[1 - nPlayerID], desc.posX[1 - nPlayerID], desc.posX[1 - nPlayerID]));

				if (players.size() == 2) {
					desc.posX[nPlayerID] = descPlayer.posX[nPlayerID];
					desc.posY[nPlayerID] = descPlayer.posY[nPlayerID];
					desc.posZ[nPlayerID] = descPlayer.posZ[nPlayerID];

					desc.orX[nPlayerID] = descPlayer.orX[nPlayerID];
					desc.orY[nPlayerID] = descPlayer.orY[nPlayerID];
					desc.orZ[nPlayerID] = descPlayer.orZ[nPlayerID];
					desc.orW[nPlayerID] = descPlayer.orW[nPlayerID];

					desc.dir[nPlayerID] = descPlayer.dir[nPlayerID];

					desc.paintX[nPlayerID] = descPlayer.paintX[nPlayerID];
					desc.paintY[nPlayerID] = descPlayer.paintY[nPlayerID];
					desc.paintZ[nPlayerID] = descPlayer.paintZ[nPlayerID];

					descPlayer = desc;
				}

				mapObjects.insert_or_assign(desc.nUniqueID, desc);
				break;
			}


			}
		}
	}

	if (bWaitingForConnection) {
		//Add waiting to connect screen
		return true;
	}

	NCL::net::message<GameMsg> msg;
	msg.header.id = GameMsg::Game_UpdatePlayer;
	msg << mapObjects[nPlayerID];
	Send(msg);
	return true;
}

void CourseworkGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	resourceManager->LoadMesh("cube.msh");
	resourceManager->LoadMesh("sphere.msh");
	resourceManager->LoadMesh("capsule.msh");
	resourceManager->LoadMesh("Gun.msh");
	resourceManager->LoadMesh("Male_Guard.msh");
	resourceManager->LoadMaterial("Male_Guard.mat", guardMat);
	basicTex = (OGLTexture*)resourceManager->LoadTexture("checkerboard.png");
	gunTex = (OGLTexture*)resourceManager->LoadTexture("doge.png");
	basicShader = (OGLShader*)resourceManager->LoadShader("GameTechVert.glsl", "bufferFragment.glsl");
	playerShader = (OGLShader*)resourceManager->LoadShader("SkinningVertex.glsl", "bufferFragment.glsl");

	InitCamera();
#ifdef _WIN64
	gameUI->Init();
#endif _WIN64

}
void CourseworkGame::InitialiseAnimations() {
#ifdef _WIN64

	animList[0] = resourceManager->LoadAnimation("Idle1.anm");
	animList[1] = resourceManager->LoadAnimation("Gunfire3.anm");
	animList[2] = resourceManager->LoadAnimation("StepBack1Rifle.anm");
	animList[3] = resourceManager->LoadAnimation("StepForwardRifle.anm");
	animList[4] = resourceManager->LoadAnimation("StepLeftRifle.anm");
	animList[5] = resourceManager->LoadAnimation("StepRightRifle.anm");
#endif _WIN64

}
CourseworkGame::~CourseworkGame() {
	delete resourceManager;
	delete physics;
	delete renderer;
	delete world;

	//delete playerSphere;
	delete enemySphere;
	delete JsonObject;
	delete JsonMesh;

	delete selectionObject;
	delete forwardObject;

	delete gameUI;

	delete meshMap;
	delete meshMap02;
	delete AIcleaner02;
	if (AIcleaner02 != nullptr) {
		AIcleaner02 = nullptr;
	}
	delete AIcleaner;
	if (AIcleaner != nullptr) {
		AIcleaner = nullptr;
	}
}

void CourseworkGame::UpdateCamera(const float& dt) {
	if (debug_mode && isFreeCamera) {
		world->GetMainCamera()->UpdateCamera_DEBUG(dt);
	}
	else {
		if (renderer->inSplitScreen) {
			// Player 1
			world->GetMainCamera()->UpdateCamera_TPS(dt, players[0]->GetTransform().GetPosition(), 1, renderer->inSplitScreen);
			playerControllers[0]->PlayerMovement(renderer->inSplitScreen);
			FirePaint(1);
			players[0]->fire();
		

			// Player 2
			world->GetViceCamera()->UpdateCamera_TPS(dt, players[1]->GetTransform().GetPosition(), 2, renderer->inSplitScreen);
			playerControllers[1]->PlayerMovement(renderer->inSplitScreen);
			FirePaint(2);
			players[1]->fire();
		}
		else {
			if (mapObjects.size() > 0) {
				world->GetMainCamera()->UpdateCamera_TPS(dt, players[nPlayerID]->GetTransform().GetPosition(), nPlayerID + 1);
				playerControllers[nPlayerID]->PlayerMovement(renderer->inSplitScreen);
				FirePaint(nPlayerID + 1);
				int dir = descPlayer.dir[1 - nPlayerID];
				if (players.size() == 2) {
					players[nPlayerID]->fire();
					players[1-nPlayerID]->getGun()->SetIsActive(true);

					playerControllers[static_cast<std::vector<NCL::CSC8503::GameObject*, std::allocator<NCL::CSC8503::GameObject*>>::size_type>(1) - nPlayerID]->facingDirection = (NCL::CharacterController::moveDirection)dir;
					if(players[1-nPlayerID]->getFiring()<=0)
					{
					NCL::CharacterController::CheckAnimationStateNet(players[static_cast<std::vector<NCL::CSC8503::GameObject*, std::allocator<NCL::CSC8503::GameObject*>>::size_type>(1) - nPlayerID], (NCL::CharacterController::moveDirection)dir);
					}
					//std::cout << playerControllers[static_cast<std::vector<NCL::CSC8503::GameObject*, std::allocator<NCL::CSC8503::GameObject*>>::size_type>(1) - nPlayerID]->facingDirection;
				}
				//std::cout << dir;
				//playerControllers[static_cast<std::vector<NCL::CSC8503::GameObject*, std::allocator<NCL::CSC8503::GameObject*>>::size_type>(1) - nPlayerID];
			}
			else if( debug_mode && players.size() > 0){
				world->GetMainCamera()->UpdateCamera_TPS(dt, players[0]->GetTransform().GetPosition(), 1, renderer->inSplitScreen);
				playerControllers[0]->PlayerMovement(renderer->inSplitScreen);
				FirePaint(1);
			}
				
		}
	}
}


void CourseworkGame::UpdateGame(float dt) {
	//debug_mode ? world->GetMainCamera()->UpdateCamera_DEBUG(dt) : world->GetMainCamera()->UpdateCamera(dt);
	UpdateCamera(dt);
	UpdateKeys();
	if (isNetworked)
		onUserUpdate();
	SelectObject();
	MoveSelectedObject();

	if (!renderer->inSplitScreen && !debug_mode) {
		if (players.size() == 2) {
			players[static_cast<std::vector<NCL::CSC8503::GameObject*, std::allocator<NCL::CSC8503::GameObject*>>::size_type>(1) - nPlayerID]->GetTransform().SetPosition(NCLVector3(descPlayer.posX[1 - nPlayerID], descPlayer.posY[1 - nPlayerID], descPlayer.posZ[1 - nPlayerID]));
			players[static_cast<std::vector<NCL::CSC8503::GameObject*, std::allocator<NCL::CSC8503::GameObject*>>::size_type>(1) - nPlayerID]->GetTransform().SetOrientation(Quaternion(descPlayer.orX[1 - nPlayerID], descPlayer.orY[1 - nPlayerID], descPlayer.orZ[1 - nPlayerID], descPlayer.orW[1 - nPlayerID]));
			players[static_cast<std::vector<NCL::CSC8503::GameObject*, std::allocator<NCL::CSC8503::GameObject*>>::size_type>(1) - nPlayerID]->GetRigidBody()->SetGlobalPosition(&players[static_cast<std::vector<NCL::CSC8503::GameObject*, std::allocator<NCL::CSC8503::GameObject*>>::size_type>(1) - nPlayerID]->GetTransform());
		}
	}

	if (players.size() > 0) {
		for (int i = 0; i < players.size(); i++) {
			players[i]->GetRenderObject()->GetTransform();
			players[i]->GetRenderObject()->GetTransform()->SetPosition(players[i]->GetTransform().GetPosition() - NCLVector3(0, 2, 0));
			players[i]->getGun()->GetTransform().SetPosition(players[i]->GetTransform().GetPosition() + NCLVector3(-0.5, 0.5, 0.5));
			players[i]->GetRenderObject()->GetTransform()->SetOrientation(players[i]->GetTransform().GetOrientation());
			players[i]->getGun()->GetTransform().SetOrientation(players[i]->GetTransform().GetOrientation());
			

		}
	}


	if (players.size() == 2)
	{
		currentpaint = NCLVector3(descPlayer.paintX[1 - nPlayerID], descPlayer.paintY[1 - nPlayerID], descPlayer.paintZ[1 - nPlayerID]);
		if (previouspaint != currentpaint)
		{
			//std::cout << "diff\n";
			FirePaintNet(currentpaint);
			previouspaint = currentpaint;
		}
	}

	if (!renderer->inSplitScreen && !debug_mode) {
		if (players.size() == 2) {
			descPlayer.posX[nPlayerID] = players[nPlayerID]->GetTransform().GetPosition().x;
			descPlayer.posY[nPlayerID] = players[nPlayerID]->GetTransform().GetPosition().y;
			descPlayer.posZ[nPlayerID] = players[nPlayerID]->GetTransform().GetPosition().z;

			descPlayer.orX[nPlayerID] = players[nPlayerID]->GetTransform().GetOrientation().x;
			descPlayer.orY[nPlayerID] = players[nPlayerID]->GetTransform().GetOrientation().y;
			descPlayer.orZ[nPlayerID] = players[nPlayerID]->GetTransform().GetOrientation().z;
			descPlayer.orW[nPlayerID] = players[nPlayerID]->GetTransform().GetOrientation().w;

			descPlayer.dir[nPlayerID] = playerControllers[nPlayerID]->facingDirection;
		}
		if (mapObjects.size() == 2) {
			for (auto& object : mapObjects) {
				for (int i = 0; i < players.size(); i++) {
					object.second.posX[i] = players[i]->GetTransform().GetPosition().x;
					object.second.posY[i] = players[i]->GetTransform().GetPosition().y;
					object.second.posZ[i] = players[i]->GetTransform().GetPosition().z;

					object.second.orX[i] = players[i]->GetTransform().GetOrientation().x;
					object.second.orY[i] = players[i]->GetTransform().GetOrientation().y;
					object.second.orZ[i] = players[i]->GetTransform().GetOrientation().z;
					object.second.orW[i] = players[i]->GetTransform().GetOrientation().w;

					object.second.dir[i] = playerControllers[i]->facingDirection;
					int temp = reservoir.size() - 1;
					object.second.paintX[i] = reservoir[temp].x;
					object.second.paintY[i] = reservoir[temp].y;
					object.second.paintZ[i] = reservoir[temp].z;
				}
			}
		}
	}


	/*if (mapObjects.size() == 2) {
		for (auto& object : mapObjects) {
			for (int i = 0; i < players.size(); i++) {
				object.second.posX[i] = players[i]->GetTransform().GetPosition().x;
				object.second.posY[i] = players[i]->GetTransform().GetPosition().y;
				object.second.posZ[i] = players[i]->GetTransform().GetPosition().z;

				object.second.orX[i] = players[i]->GetTransform().GetOrientation().x;
				object.second.orY[i] = players[i]->GetTransform().GetOrientation().y;
				object.second.orZ[i] = players[i]->GetTransform().GetOrientation().z;
				object.second.orW[i] = players[i]->GetTransform().GetOrientation().w;
			}
		}
	}*/

	physics->Update(dt);

	if (lockedObject != nullptr) {
		Maths::NCLVector3 objPos = lockedObject->GetTransform().GetPosition();
		Maths::NCLVector3 camPos = objPos + lockedOffset;

		NCLMatrix4 temp = NCLMatrix4::BuildViewMatrix(camPos, objPos, Maths::NCLVector3(0.0f, 1.0f, 0.0f));

		NCLMatrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Maths::NCLVector3 angles = q.ToEuler();

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}

	if (AIcleaner) {
		UpdatePaintAndCleaner();
		AIcleaner->Update(dt);
	}

	if (AIcleaner02) {
		UpdatePaintAndCleaner02();
		AIcleaner02->Update(dt);
	}

	world->UpdateWorld(dt);


	if (players.size() == 2)
		audio::UpdateAudio(players[0]->GetTransform().GetPosition(), players[1]->GetTransform().GetPosition());
	renderer->Update(dt);



	renderer->Render();
	if (debug_mode) {
		physics->RenderDebugInfo();
	}
	ShowRigidBodyStatus();
#ifdef _x64
	Debug::FlushRenderables(dt);
#endif
	fireTime -= dt;

}

void CourseworkGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1)) {
		//LoadLevel();
		selectionObject = nullptr;
		lockedObject = nullptr;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2)) {
		//LoadConvexMeshTest();
		selectionObject = nullptr;
		lockedObject = nullptr;
	}
	// Shuffle to reduce constraint bias calculations
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (debug_mode) {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F5)) {
			isFreeCamera = !isFreeCamera;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F6)) {
			renderer->ShowNavMesh = !renderer->ShowNavMesh;
		}
	}


	if (lockedObject) {
		LockedObjectMovement();
	}
}

void CourseworkGame::LockedObjectMovement() {
	NCLMatrix4 view = world->GetMainCamera()->BuildViewMatrix();
	NCLMatrix4 camWorld = view.Inverse();

	Maths::NCLVector3 rightAxis = Maths::NCLVector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Maths::NCLVector3 fwdAxis = Maths::NCLVector3::Cross(Maths::NCLVector3(0.0f, 1.0f, 0.0f), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	Maths::NCLVector3 charForward = lockedObject->GetTransform().GetOrientation() * Maths::NCLVector3(0.0f, 0.0f, 1.0f);
	Maths::NCLVector3 charForward2 = lockedObject->GetTransform().GetOrientation() * Maths::NCLVector3(0.0f, 0.0f, 1.0f);

	float force = 200.0f;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		lockedObject->GetRigidBody()->AddForce(-rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		Maths::NCLVector3 worldPos = selectionObject->GetTransform().GetPosition();
		lockedObject->GetRigidBody()->AddForce(rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		lockedObject->GetRigidBody()->AddForce(fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		lockedObject->GetRigidBody()->AddForce(-fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		lockedObject->GetRigidBody()->AddForce(Maths::NCLVector3(0, -10, 0));
	}
}

void CourseworkGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(1500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Maths::NCLVector3(-60.0f, 40.0f, 60.0f));
	lockedObject = nullptr;

	//vice camera
	world->GetViceCamera()->SetNearPlane(0.1f);
	world->GetViceCamera()->SetFarPlane(1500.0f);
	lockedObject = nullptr;
}

bool CourseworkGame::startReady() {
	if (!renderer->inSplitScreen) {
		if (players.size() == 2)
			return true;
		else
			return false;
	}
	else
		return true;
}

void CourseworkGame::LoadPlayer() {
	if (mapObjects.size() <= 2) {
		for (auto& object : mapObjects) {
			if (numplayers < mapObjects.size()) {
				numplayers++;
				NCLVector3 spawnpoint = (numplayers == 1) ? (meshMap->allTris[5].centroid + NCLVector3(100, 1.0f, -105)) : (meshMap->allTris[8].centroid + NCLVector3(-10, 1.0f, -105));
				players.emplace_back(AddPlayerToWorld(spawnpoint, 2, 1));
				playerControllers[players.size() - 1] = new CharacterController(world, players[players.size() - 1], nPlayerID + 1, physics);
			}
			else
				break;
		}
		playernum = nPlayerID;

	}
}

void CourseworkGame::LoadLevel(int i) {
	currentLevel = 1;
	players.clear();

	Maths::NCLVector3 pos = Maths::NCLVector3();
	Maths::NCLVector3 dimensions = Maths::NCLVector3(5, 5, 5);

	//AddFloorToWorld(Maths::NCLVector3(0, 0, 0), Maths::NCLVector3(100,1,100));
	world->GetViceCamera()->SetPosition(NCLVector3(-10.6f, 10.6f, -15.8f));
	world->GetViceCamera()->SetPitch(-10.3f);
	world->GetViceCamera()->SetYaw(138.0f);

	//AddFloorToWorld(NCLVector3((meshMap->maxX + meshMap->minX) / 2, -9, (meshMap->maxZ + meshMap->minZ) / 2), NCLVector3((meshMap->maxX - meshMap->minX) / 2, 1, (meshMap->maxZ - meshMap->minZ) / 2));
	InitNavMeshAssets();

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-15, 8.5, -8),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-21, 8.5, -8),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-27, 8.5, -8),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-33, 8.5, -8),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-39, 8.5, -8),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-45, 8.5, -8),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, -8),
		NCLVector3(2, 1, 1),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, -5),
		NCLVector3(4, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, 2),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, 8),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, 14),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, 20),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, 26),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, 42),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, 48),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, 54),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-50, 8.5, 60),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-61, 8.5, 36),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-61, 8.5, 30),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-61, 8.5, 24),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-61, 8.5, 18),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-61, 8.5, 12),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-61, 8.5, 6),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-57, 8.5, 38),
		NCLVector3(5, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-47, 8.5, 59),
		NCLVector3(4, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-40, 8.5, 59),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-34, 8.5, 59),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-28, 8.5, 59),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-22, 8.5, 59),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-16, 8.5, 59),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-10, 8.5, 59),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-4, 8.5, 59),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(2, 8.5, 59),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(8, 8.5, 59),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(15, 8.5, 59),
		NCLVector3(4, 1, 1),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, 51),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, 45),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, 39),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, 33),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, 27),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, 21),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, 15),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, 9),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, 3),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, -3),
		NCLVector3(3, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("inner_sanctum_max_wall_straight_mid",
		NCLVector3(-14, 8.5, -7),
		NCLVector3(2, 1, 1),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-49, 0.5, 25),
		NCLVector3(35, 1.32, 2),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-15, 0.5, 25),
		NCLVector3(35, 1.32, 2),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-32, 0.5, -7),
		NCLVector3(20, 1.32, 2),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-66, 0.5, 2),
		NCLVector3(37, 1.32, 2),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(22, 0.5, 38),
		NCLVector3(23, 1.32, 2),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(3, 0.5, 64),
		NCLVector3(16, 1.32, 2),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-15.5, 0.5, -96),
		NCLVector3(40, 1.32, 2),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-75, 0.5, -89),
		NCLVector3(19.5, 1.32, 2),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-75, 0.5, -57),
		NCLVector3(19.5, 1.32, 2),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(44, 0.5, -89),
		NCLVector3(19.5, 1.32, 2),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(44, 0.5, -57),
		NCLVector3(19.5, 1.32, 2),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(67, 0.5, -73),
		NCLVector3(13, 1.32, 2),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-98, 0.5, -73),
		NCLVector3(13, 1.32, 2),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-46.5, 0.5, -38.5),
		NCLVector3(16, 1.32, 2),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(8.5, 0.5, -38.5),
		NCLVector3(9, 1.32, 2),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(20, 0.5, -30),
		NCLVector3(5, 1.32, 2),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(28, 0.5, -29),
		NCLVector3(10, 1.32, 2),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(41, 0.5, -5),
		NCLVector3(20, 1.32, 2),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(28, 0.5, 18),
		NCLVector3(9, 1.32, 2),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-2, 0.5, 48),
		NCLVector3(10, 1.32, 2),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-2, 0.5, 44),
		NCLVector3(10, 1.32, 2),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(4, 0.5, 46),
		NCLVector3(4, 1.32, 2),
		NCLVector3(0, 90, 0));

	
	//AddCubeToWorld(NCLVector3(-3, 7, 46), NCLVector3(10, 1, 5), 0.0f, false, true, false);
	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-2, 4, 52),
		NCLVector3(9, 2, 2),
		NCLVector3(270, 0, 0));

	

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(3, 0.5, -44),
		NCLVector3(9, 1.32, 2),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-34, 0.5, -44),
		NCLVector3(9, 1.32, 2),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-43.5, 0.5, -50),
		NCLVector3(12.5, 1.32, 2),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(12, 0.5, -50),
		NCLVector3(12.5, 1.32, 2),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(28, 0.5, -57),
		NCLVector3(3, 1.32, 2),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(28, 0.5, -89),
		NCLVector3(3, 1.32, 2),
		NCLVector3(0, 270, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-59, 0.5, -57),
		NCLVector3(3, 1.32, 2),
		NCLVector3(0, 90, 0));

	LoadEvilGeniusScene("corridor_Wall_Straight_Mid_end_L",
		NCLVector3(-59, 0.5, -89),
		NCLVector3(3, 1.32, 2),
		NCLVector3(0, 90, 0));


	LoadEvilGeniusScene("SanctumCat",
		NCLVector3(-18, 16, -75),
		NCLVector3(20, 20, 20),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("SanctumThrone",
		NCLVector3(-32, 8.5, 0),
		NCLVector3(2, 2, 2),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("SanctumDesk",
		NCLVector3(-32, 8.5, 5),
		NCLVector3(2, 2, 2),
		NCLVector3(0, 0, 0));

	LoadEvilGeniusScene("Corridor_Wall_Light",
		NCLVector3(-32, 0, -14),
		NCLVector3(1.5, 1.5, 1.5),
		NCLVector3(0, 180, 0));

	LoadEvilGeniusScene("SanctumCat",
		NCLVector3(20, 13, -7),
		NCLVector3(15, 15, 15),
		NCLVector3(0, 270, 0));

	AddKillPlaneToWorld(NCLVector3(0, -250, 0));

	if (config->GetVariable("physicsScene")) {
		if (i == 1) {
			renderer->inSplitScreen = true;
			players.emplace_back(AddPlayerToWorld(meshMap->allTris[5].centroid + NCLVector3(100, 1.0f, -105), 2, 1, 50.0f));
			players.emplace_back(AddPlayerToWorld(meshMap->allTris[8].centroid + NCLVector3(-10, 1.0f, -105), 2, 1, 50.0f));
			// add player controllers
			playerControllers[0] = new CharacterController(world, players[0], 1, physics);
			playerControllers[1] = new CharacterController(world, players[1], 2, physics);

			float size = 1;
			float halfExtent = 2;
			Maths::NCLVector3 boxPos = Maths::NCLVector3(20.0, 1.0f, 20.0f);

		}
		AddPowerupToWorld(NCLVector3(10, 5, -70), "Dash");
		AddPowerupToWorld(NCLVector3(-60, 5, -70), "Dash");
		AddPowerupToWorld(NCLVector3(3, 5, 10), "Speed");

		JsonHandler::ReadFile("levelnowalls.json", this);
	}

	//Light position
	//AddCubeToWorld(NCLVector3(-50.0f, 50.0f, 200.0f); , NCLVector3(5, 5, 5), 10.0f, false, true);
}

void CourseworkGame::LoadConvexMeshTest() {
	players.emplace_back(AddPlayerToWorld(NCLVector3(0.0f, 20.0f, 0.0f), 4, 2));
	players.emplace_back(AddPlayerToWorld(NCLVector3(0.0f, 20.0f, 0.0f), 4, 2));
	AddFloorToWorld(NCLVector3(0, 0, 0), NCLVector3(100, 1, 100));

	GameObject* model = new GameObject("character");
	Maths::NCLVector3 pos = Maths::NCLVector3(20.0, 10.0f, 20.0f);

	model->GetTransform()
		.SetPosition(pos)
		.SetScale(Maths::NCLVector3(10, 10, 10));

	RigidBody* body = new RigidBody(&model->GetTransform());

	MeshGeometry* charMeshA = resourceManager->LoadMesh("Male1.msh");
	body->AddConvexMesh(charMeshA);
	model->SetRenderObject(new RenderObject(&model->GetTransform(), charMeshA, basicTex, basicShader));
	model->SetPhysicsObject(body);
	physics->AddRigidBody(body);

	world->AddGameObject(model);
}



void CourseworkGame::InitNavMeshAssets() {
	//Init AI
	NCLVector3 start = meshMap->allTris[0].centroid;
	//std::cout << meshMap->mapSize << std::endl;
	//std::cout << meshMap->maxX << "," << meshMap->maxZ << "," << meshMap->minX << "," << meshMap->minZ << std::endl;
	//std::cout << "start : " << start.x << "," << start.y << "," << start.z << std::endl;
	AddCleanerToWorld(start);

	NCLVector3 start02 = meshMap02->allTris[8].centroid;
	AddCleaner02ToWorld(start02);
}

GameObject* CourseworkGame::AddFloorToWorld(const Maths::NCLVector3& position, const Maths::NCLVector3& floorSize, const Maths::NCLVector4& colour, bool isSpring) {
	GameObject* floor = new GameObject("Floor");
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);
	RigidBody* body = new RigidBody(&floor->GetTransform(), true);
	body->AddBoxShape(floorSize);

	MeshGeometry* cubeMesh = resourceManager->LoadMesh("cube.msh");
	vector<TextureBase*>textures;
	textures.push_back(resourceManager->LoadTexture("dirt_Diffuse.tga"));
	textures.push_back(resourceManager->LoadTexture("dirt_Normal.tga"));
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, textures, basicShader, colour));
	floor->SetPhysicsObject(body);
	physics->AddRigidBody(body);

	world->AddGameObject(floor);

	return floor;
}

GameObject* CourseworkGame::AddSphereToWorld(const Maths::NCLVector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject("Sphere");
	Maths::NCLVector3 sphereSize = Maths::NCLVector3(radius, radius, radius);

	sphere->GetTransform()
		.SetPosition(position)
		.SetScale(sphereSize);

	RigidBody* body = new RigidBody(&sphere->GetTransform());
	body->AddSphereShape(radius);

	MeshGeometry* sphereMesh = resourceManager->LoadMesh("sphere.msh");
	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, nullptr, basicShader));
	sphere->SetPhysicsObject(body);
	physics->AddRigidBody(body);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* CourseworkGame::AddCapsuleToWorld(const Maths::NCLVector3& position, float halfHeight, float radius, float inverseMass, bool isSpring) {
	GameObject* capsule = new GameObject();
	capsule->SetName("Capsule");
	capsule->GetTransform()
		.SetPosition(position)
		.SetScale(Maths::NCLVector3(radius * 2, halfHeight, radius * 2));

	RigidBody* body = new RigidBody(&capsule->GetTransform());
	body->SetInverseMass(inverseMass);
	body->AddCapsuleShape(radius, halfHeight);

	MeshGeometry* capsuleMesh = resourceManager->LoadMesh("capsule.msh");
	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, playerShader));
	capsule->SetPhysicsObject(body);
	physics->AddRigidBody(body);

	world->AddGameObject(capsule);

	return capsule;
}

GameObject* NCL::CSC8503::CourseworkGame::AddKillPlaneToWorld(const Maths::NCLVector3& position) {
	GameObject* plane = new GameObject("kill plane");
	Maths::NCLVector3 dimensions = Maths::NCLVector3(500, 1, 500);

	plane->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	RigidBody* body = new RigidBody(&plane->GetTransform(), true);
	body->AddBoxShape(dimensions);
	body->SetTrigger(true);
	//body->VisualizeDebugVolume(false);
	body->SetIsRaycastable(false);
	plane->SetTriggerFunc([&](GameObject* otherActor)->void
		{
			if (otherActor->GetName() == "Player") {
				Transform* trans = &otherActor->GetTransform();
				trans->SetPosition(NCLVector3(0, 5, 0));
				otherActor->GetRigidBody()->SetGlobalPosition(trans);
			}
			else {
				otherActor->SetToDelete(true);
			}
		}
	);
	body->SetCollisionFilters(CollisionLayers::Default, CollisionLayers::ALL);

	plane->SetPhysicsObject(body);
	physics->AddRigidBody(body);
	world->AddGameObject(plane);
	return plane;
}

Cleaner* CourseworkGame::AddCleanerToWorld(const NCLVector3& position) {
	AIcleaner = new Cleaner(meshMap, testPaint);
	AIcleaner->SetName("Cleaner");

	NCLVector3 dimensions = NCLVector3(5.0f, 1.5f, 5.0f);
	AIcleaner->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	RigidBody* body = new RigidBody(&AIcleaner->GetTransform());
	body->AddBoxShape(dimensions);
	body->SetGravityDisable(true);
	body->Lock(RigidBodyConstraints::LockRotX | RigidBodyConstraints::LockRotZ);
	body->SetTrigger(true);
	AIcleaner->SetTriggerFunc([&](GameObject* otherActor)->void
		{
			if (otherActor->GetName() == "Player") {
				/*std::cout << "collision happened" << std::endl;*/
			}
		}
	);
	body->SetInverseMass(1.0f);
	body->SetCollisionFilters(CollisionLayers::Group2, CollisionLayers::Default);

	MeshGeometry* cleanerMesh = resourceManager->LoadMesh("OrangeSpider.msh");
	AIcleaner->SetRenderObject(new RenderObject(&AIcleaner->GetTransform(), cleanerMesh, resourceManager->LoadTexture("metal.png"), basicShader));

	AIcleaner->SetPhysicsObject(body);
	physics->AddRigidBody(body);

	world->AddGameObject(AIcleaner);

	return AIcleaner;
}

Cleaner* CourseworkGame::AddCleaner02ToWorld(const NCLVector3& position) {
	AIcleaner02 = new Cleaner(meshMap02, testPaint02);
	AIcleaner02->SetName("Cleaner");

	NCLVector3 dimensions = NCLVector3(5.0f, 1.5f, 5.0f);
	AIcleaner02->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	RigidBody* body = new RigidBody(&AIcleaner02->GetTransform());
	body->AddBoxShape(dimensions);
	body->SetGravityDisable(true);
	body->Lock(RigidBodyConstraints::LockRotX | RigidBodyConstraints::LockRotZ);
	body->SetTrigger(true);
	AIcleaner02->SetTriggerFunc([&](GameObject* otherActor)->void
		{
			if (otherActor->GetName() == "Player") {
				/*std::cout << "collision happened" << std::endl;*/
			}
		}
	);
	body->SetInverseMass(1.0f);
	body->SetCollisionFilters(CollisionLayers::Group2, CollisionLayers::Default);

	//MeshGeometry* cleanerMesh = resourceManager->LoadMesh("Drone_Guard.msh");
	//vector<TextureBase*> textures;
	//resourceManager->LoadMaterial("Drone_Guard.mat", textures);

/*MeshGeometry* cleanerMesh = resourceManager->LoadMesh("cube.msh");
AIcleaner->SetRenderObject(new RenderObject(&AIcleaner->GetTransform(), cleanerMesh, resourceManager->LoadTexture("doge.png"), basicShader));*/


	MeshGeometry* cleanerMesh = resourceManager->LoadMesh("OrangeSpider.msh");
	AIcleaner02->SetRenderObject(new RenderObject(&AIcleaner02->GetTransform(), cleanerMesh, resourceManager->LoadTexture("nyan.png"), basicShader));

	AIcleaner02->SetPhysicsObject(body);
	physics->AddRigidBody(body);

	world->AddGameObject(AIcleaner02);

	return AIcleaner02;
}

void CourseworkGame::ShowRigidBodyStatus() {
	if (debug_mode && changeColour) {
		world->OperateOnContents([&](GameObject* obj)->void
			{
				RenderObject* renderObj = obj->GetRenderObject();
				RigidBody* body = obj->GetRigidBody();
				if (renderObj && body) {
					if (body->IsStatic()) renderObj->SetColour(Debug::RED);
					else if (body->IsAsleep()) renderObj->SetColour(Debug::YELLOW);
					else renderObj->SetColour(Debug::GREEN);
				}
			});
	}
	else if (!changeColour) {
		world->OperateOnContents([&](GameObject* obj)->void
			{
				RenderObject* renderObj = obj->GetRenderObject();
				RigidBody* body = obj->GetRigidBody();
				if (renderObj && body) {
					if (renderObj->GetDefaultColour() != NCLVector4()) {
						renderObj->SetColour(renderObj->GetDefaultColour());
					}
					else {
						renderObj->SetColour(NCLVector4(1, 1, 1, 1));
					}
				}

			});
		changeColour = false;
	}
}

GameObject* CourseworkGame::AddCubeToWorld(const NCLVector3& position, NCLVector3 dimensions, float inverseMass, bool isSpring, bool isStatic, bool isAItest) {
	GameObject* cube = new GameObject();
	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	RigidBody* body = new RigidBody(&cube->GetTransform(), isStatic);

	body->SetInverseMass(inverseMass);
	body->AddBoxShape(dimensions);

	cube->SetName("Cube");

	MeshGeometry* cubeMesh = resourceManager->LoadMesh("cube.msh");
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(body);
	physics->AddRigidBody(body);

	world->AddGameObject(cube);

	return cube;
}

PlayerObject* CourseworkGame::AddPlayerToWorld(const NCLVector3& position, float halfHeight, float radius, float inverseMass, bool isSpring) {
	PlayerObject* capsule = new PlayerObject();
	capsule->SetName("Player");
	capsule->GetTransform()
		.SetPosition(position)
		.SetScale(Maths::NCLVector3(radius * 2, halfHeight * 1.2, radius * 2));

	RigidBody* body = new RigidBody(&capsule->GetTransform());
	body->SetInverseMass(inverseMass);
	body->AddCapsuleShape(radius, halfHeight / 2, true);
	body->Lock(RigidBodyConstraints::LockRotX | RigidBodyConstraints::LockRotZ);
	body->SetLinearDamping(0.3f);
	body->SetStaticFriction(0.35f);
	body->SetDynamicFriction(0.3f);
	body->SetRestitution(0.0f);
	body->SetCollisionFilters(CollisionLayers::Default | CollisionLayers::Group1, CollisionLayers::Default | CollisionLayers::Group1);
	body->SetIsRaycastable(false);

	MeshGeometry* capsuleMesh = resourceManager->LoadMesh("Male_Guard.msh");
	Transform* t = new Transform(capsule->GetTransform());
	t->SetPosition(capsule->GetTransform().GetPosition() - NCLVector3(0, 10, 0));
	capsule->SetRenderObject(new RenderObject(t, capsuleMesh, guardMat, playerShader));
	capsule->SetPhysicsObject(body);
	physics->AddRigidBody(body);
	capsule->GetRenderObject()->SetAnimationList(animList);
	capsule->GetRenderObject()->SetAnimation(animList[0]);
	capsule->GetRenderObject()->setAnimationIndex(0);
	capsule->setGun(AddGun(position));
	world->AddGameObject(capsule);

	return capsule;
}

GameObject* NCL::CSC8503::CourseworkGame::AddPowerupToWorld(const NCLVector3& position, string name)
{
	GameObject* coin = new GameObject(name);

	coin->GetTransform()
		.SetScale(NCLVector3(0.7, 0.7, 0.7))
		.SetPosition(position);

	RigidBody* body = new RigidBody(&coin->GetTransform());
	body->SetInverseMass(1);
	body->AddSphereShape(1.2f);
	body->Lock(RigidBodyConstraints::LockRotX | RigidBodyConstraints::LockRotZ);
	body->Lock(RigidBodyConstraints::LockPosY);
	body->SetCollisionFilters(CollisionLayers::Group1, CollisionLayers::Default | CollisionLayers::Group1);

	MeshGeometry* coinMesh = resourceManager->LoadMesh("sphere.msh");

	TextureBase* tex;
	if (name == "Dash") {
		tex = resourceManager->LoadTexture("nyan.png");
	}
	else {
		tex = resourceManager->LoadTexture("doge.png");
	}

	coin->SetRenderObject(new RenderObject(&coin->GetTransform(), coinMesh, tex, basicShader));
	coin->SetPhysicsObject(body);

	physics->AddRigidBody(body);
	world->AddGameObject(coin);

	return coin;
}

GameObject* CourseworkGame::AddGun(const NCLVector3& position)
{
	GameObject* gun = new GameObject();
	gun->GetTransform()
		.SetPosition(position)
		.SetScale(NCLVector3(2,2,2));
	MeshGeometry* gunMesh = resourceManager->LoadMesh("Gun.msh");
	gun->SetRenderObject(new RenderObject(&gun->GetTransform(), gunMesh, gunTex, basicShader));
	world->AddGameObject(gun);
	gun->SetIsActive(false);
	return gun;
}

bool CourseworkGame::SelectObject() {
	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
		if (selectionObject) {	//set colour to deselected;
			selectionObject->GetRenderObject()->SetColour(Maths::NCLVector4(1.0f, 1.0f, 1.0f, 1.0f));
			selectionObject = nullptr;
			lockedObject = nullptr;
		}
		// Unhighlighting for functionality to fire forward collision
		if (forwardObject) {	//set colour to deselected;
			forwardObject->GetRenderObject()->SetColour(Maths::NCLVector4(1, 1, 1, 1));
			forwardObject = nullptr;
			lockedObject = nullptr;
		}


		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (physics->Raycast(ray, closestCollision)) {
#ifdef _x64
			/*Debug::DrawLine(ray.GetPosition(), closestCollision.collidedAt, Debug::RED, 10.0f);*/
#endif
			//selectionObject = (GameObject*)closestCollision.node;
		//	selectionObject->GetRenderObject()->SetColour(Maths::NCLVector4(0.0f, 1.0f, 0.0f, 1.0f));

			return true;
		}
		else {
			return false;
		}
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
		}
	}
	return false;
}

void CourseworkGame::MoveSelectedObject() {
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;// we haven t selected anything !
	}

	// Push the selected object !
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;
		if (physics->Raycast(ray, closestCollision)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetRigidBody()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::D)) {
		selectionObject->GetRigidBody()->AddForce(Maths::NCLVector3(1, 0, 0) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::S)) {
		selectionObject->GetRigidBody()->AddForce(Maths::NCLVector3(0, 0, 1) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::W)) {
		selectionObject->GetRigidBody()->AddForce(Maths::NCLVector3(0, 0, -1) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::A)) {
		selectionObject->GetRigidBody()->AddForce(Maths::NCLVector3(-1, 0, 0) * forceMagnitude);
	}
}

void CourseworkGame::LoadEvilGeniusScene(string filename, const Maths::NCLVector3& position, Maths::NCLVector3 dimensions, Maths::NCLVector3 rotation) {
	GameObject* cube = new GameObject();
	cube->SetName("Cube");
	cube->GetTransform()
		.SetPosition(position)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(rotation.x, rotation.y, rotation.z))
		.SetScale(dimensions * 2);

	RigidBody* body = new RigidBody(&cube->GetTransform(), true);

	vector<TextureBase*> textures;
	resourceManager->LoadMaterial(filename + ".mat", textures);

	MeshGeometry* cubeMesh = resourceManager->LoadMesh(filename + ".msh");
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, textures, basicShader));
	cube->SetPhysicsObject(body);
	body->AddConvexMesh(cubeMesh);
	body->SetCollisionFilters(CollisionLayers::Default, CollisionLayers::Default | CollisionLayers::Paint);
	physics->AddRigidBody(body);
	world->AddGameObject(cube);
}

GameObject* NCL::CSC8503::CourseworkGame::AddPaintToWorld(const NCLVector3& position, const Quaternion& orientation, int playerID) {
	GameObject* paint = new GameObject("Paint splat");
	NCLVector3 dimensions = NCLVector3(0.3, 0.03, 0.3);

	MeshGeometry* cubeMesh = resourceManager->LoadMesh("sphere.msh");
	paint->SetRenderObject(new RenderObject(&paint->GetTransform(), cubeMesh, nullptr, basicShader));
	if (playerID == 1) {
		paint->GetRenderObject()->SetColour(NCLVector4(1, 0, 0, 1));
	}
	else {
		paint->GetRenderObject()->SetColour(NCLVector4(0, 0, 1, 1));
	}

	paint->GetTransform()
		.SetPosition(position)
		.SetOrientation(orientation)
		.SetScale(dimensions * 5);

	world->AddGameObject(paint);

	return paint;
}

// 'Applying paint' by placing a cube that will apply the paint decal to the surface
GameObject* CourseworkGame::AddPaintPlacingCube(const NCLVector3& position, const Quaternion& orientation, int playerID) {
	GameObject* paint = new GameObject("Paint splat");
	paint->SetIsPaintPlacingCube(true);
	NCLVector3 dimensions = NCLVector3(2, 0.5, 2);

	paint->GetTransform()
		.SetPosition(position)
		.SetOrientation(orientation)
		.SetScale(dimensions * 2);

		MeshGeometry* cubeMesh = resourceManager->LoadMesh("cube.msh");
		paint->SetRenderObject(new RenderObject(&paint->GetTransform(), cubeMesh, basicTex, basicShader));
		if (playerID == 1) {
			paint->GetRenderObject()->SetColour(NCLVector4(1, 0, 0, 1.0f));
			paint->GetRenderObject()->SetDefaultColour(NCLVector4(1, 0, 0, 1.0f));
		}
		else {
			paint->GetRenderObject()->SetColour(NCLVector4(0, 0, 1, 1.0f));
			paint->GetRenderObject()->SetDefaultColour(NCLVector4(0, 0, 1, 1.0f));
		}

	world->AddGameObject(paint);

	return paint;
}

void CourseworkGame::FirePaint(int playerID) {
	float heightPosOffset = 0.5f;
	sf::Joystick::update();
	//float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);

	//if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT) || sf::Joystick::isButtonPressed(0, 5)) {
	if (renderer->inSplitScreen) {
		if ((Window::GetMouse()->ButtonHeld(NCL::MouseButtons::LEFT) && playerID == 1 || sf::Joystick::isButtonPressed(0, 5) && playerID == 2) && fireTime <= 0.0f) {
			audio::PlayShoot(playerID - 1);

			//Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
			NCLVector3 pos;
			NCLMatrix4 view;
			NCLVector3 direction;
			Ray ray2 = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			NCLVector3 pos1;
			NCLVector3 pos2;

			switch (playerID) {
			case 1:
				pos = players[0]->GetTransform().GetPosition();
				pos = NCLVector3(pos.x, pos.y + heightPosOffset, pos.z);

				direction = ray2.GetDirection();
				view = world->GetMainCamera()->BuildViewMatrix();

				//pos1 = world->GetMainCamera()->GetPosition() - playerSphere->GetTransform().GetPosition();
				pos1 = players[0]->GetTransform().GetPosition() - world->GetMainCamera()->GetPosition();
				pos1.Length();
				pos1.Normalise();
				players[0]->GetRenderObject()->SetAnimation(animList[1]);
				players[0]->setFiring();
				players[0]->getGun()->SetIsActive(true);
				//direction = pos1;

				//NCLVector3::Length(playerSphere->GetTransform().GetPosition(), player2Sphere->GetTransform().GetPosition());
				break;
			case 2:
				pos = players[1]->GetTransform().GetPosition();
				pos = NCLVector3(pos.x, pos.y + heightPosOffset, pos.z);
				view = world->GetViceCamera()->BuildViewMatrix();

				pos2 = players[1]->GetTransform().GetPosition() - world->GetViceCamera()->GetPosition();
				pos2.Length();
				pos2.Normalise();
				direction = pos2;
				players[1]->GetRenderObject()->SetAnimation(animList[1]);
				players[1]->setFiring();
				players[1]->getGun()->SetIsActive(true);
				break;
			default:
				break;
			}

			NCLMatrix4 camWorld = view.Inverse();
			NCLVector3 rightAxis = NCLVector3(camWorld.GetColumn(0));

			NCLVector3 fwdAxis = NCLVector3::Cross(NCLVector3(1.0f, 1.0f, 0.0f), rightAxis);
			fwdAxis.Normalise();

			if (!config->GetVariable("oldPaint")) {
				GameObject* bullet = AddSphereToWorld(pos, 0.5f);
				bullet->GetRigidBody()->SetCollisionFilters(CollisionLayers::Paint, CollisionLayers::Default);
				bullet->SetOnCollisionBeingFunc([=](GameObject* otherActor, ContactPoint contact)->void
					{
						audio::PlayStick(contact.localA);

						GameObject* newPaint = AddPaintPlacingCube(contact.localA, Quaternion(), playerID);
						newPaint->GetTransform().SetOrientation(Quaternion::FromToRotation(NCLVector3(0, 1, 0), contact.normal) * newPaint->GetTransform().GetOrientation());
						if (newPaint->GetTransform().GetPosition().x <= meshMap->maxX && newPaint->GetTransform().GetPosition().x >= meshMap->minX
							&& newPaint->GetTransform().GetPosition().z >= meshMap->minZ && newPaint->GetTransform().GetPosition().z <= meshMap->maxZ
							&& newPaint->GetTransform().GetPosition().y >= meshMap->minY && newPaint->GetTransform().GetPosition().y <= meshMap->maxY) {
							testPaint->emplace_back(newPaint);
						}
						if (newPaint->GetTransform().GetPosition().x <= meshMap02->maxX && newPaint->GetTransform().GetPosition().x >= meshMap02->minX
							&& newPaint->GetTransform().GetPosition().z >= meshMap02->minZ && newPaint->GetTransform().GetPosition().z <= meshMap02->maxZ
							&& newPaint->GetTransform().GetPosition().y >= meshMap02->minY && newPaint->GetTransform().GetPosition().y <= meshMap02->maxY) {
							testPaint02->emplace_back(newPaint);
						}

						playerID == 1 ? redPaint++ : bluePaint++;
						bullet->SetToDelete(true);
					}
				);
				bullet->GetRigidBody()->ApplyLinearImpulse(direction * NCLVector3(3, 3, 3));
			//	bullet->GetRigidBody()->ApplyLinearImpulse(NCLVector3(-1,0,0) * NCLVector3(5, 5, 5));
				bullet->GetRenderObject()->SetColour(playerID == 1 ? Debug::RED : Debug::BLUE);
				bullet->GetRenderObject()->SetDefaultColour(playerID == 1 ? Debug::RED : Debug::BLUE);
				fireTime = FIRE_TIMER;
			}
		}
	}
	else {
		if ((Window::GetMouse()->ButtonHeld(NCL::MouseButtons::LEFT) || sf::Joystick::isButtonPressed(0, 5)) && fireTime <= 0.0f) {
			audio::PlayShoot(playerID - 1);

			//Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
			NCLVector3 pos;
			NCLMatrix4 view;
			NCLVector3 direction;
			Ray ray2 = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			NCLVector3 pos1;
			NCLVector3 pos2;

			pos = players[nPlayerID]->GetTransform().GetPosition();
			pos = NCLVector3(pos.x, pos.y + heightPosOffset, pos.z);

			//direction = ray2.GetDirection();
			view = world->GetMainCamera()->BuildViewMatrix();

			//pos1 = world->GetMainCamera()->GetPosition() - playerSphere->GetTransform().GetPosition();
			pos1 = players[nPlayerID]->GetTransform().GetPosition() - world->GetMainCamera()->GetPosition();
			pos1.Length();
			pos1.Normalise();
			players[nPlayerID]->GetRenderObject()->SetAnimation(animList[1]);
			players[nPlayerID]->setFiring();
			players[nPlayerID]->getGun()->SetIsActive(true);
			direction = pos1;

				//NCLVector3::Length(playerSphere->GetTransform().GetPosition(), player2Sphere->GetTransform().GetPosition());
			

			NCLMatrix4 camWorld = view.Inverse();
			NCLVector3 rightAxis = NCLVector3(camWorld.GetColumn(0)); //view is inverse of model!

			NCLVector3 fwdAxis = NCLVector3::Cross(NCLVector3(1.0f, 1.0f, 0.0f), rightAxis);
			fwdAxis.Normalise();

			if (!config->GetVariable("oldPaint")) {
				GameObject* bullet = AddSphereToWorld(pos, 0.5f);
				bullet->GetRigidBody()->SetCollisionFilters(CollisionLayers::Paint, CollisionLayers::Default);
				bullet->SetOnCollisionBeingFunc([=](GameObject* otherActor, ContactPoint contact)->void
					{
						GameObject* newPaint = AddPaintPlacingCube(contact.localA, Quaternion(), playerID);
						newPaint->GetTransform().SetOrientation(Quaternion::FromToRotation(NCLVector3(0, 1, 0), contact.normal) * newPaint->GetTransform().GetOrientation());
						if (newPaint->GetTransform().GetPosition().x <= meshMap->maxX && newPaint->GetTransform().GetPosition().x >= meshMap->minX
							&& newPaint->GetTransform().GetPosition().z >= meshMap->minZ && newPaint->GetTransform().GetPosition().z <= meshMap->maxZ
							&& newPaint->GetTransform().GetPosition().y >= meshMap->minY && newPaint->GetTransform().GetPosition().y <= meshMap->maxY) {
							testPaint->emplace_back(newPaint);
						}
						if (newPaint->GetTransform().GetPosition().x <= meshMap02->maxX && newPaint->GetTransform().GetPosition().x >= meshMap02->minX
							&& newPaint->GetTransform().GetPosition().z >= meshMap02->minZ && newPaint->GetTransform().GetPosition().z <= meshMap02->maxZ
							&& newPaint->GetTransform().GetPosition().y >= meshMap02->minY && newPaint->GetTransform().GetPosition().y <= meshMap02->maxY) {
							testPaint02->emplace_back(newPaint);
						}

						playerID == 1 ? redPaint++ : bluePaint++;
						bullet->SetToDelete(true);
					}
				);
				descPlayer.paintX[nPlayerID] = direction.x;
				descPlayer.paintY[nPlayerID] = direction.y;
				descPlayer.paintZ[nPlayerID] = direction.z;
				reservoir.emplace_back(direction);
				bulletcolour = playerID == 1 ? Debug::RED : Debug::BLUE;
				bullet->GetRigidBody()->ApplyLinearImpulse(direction * NCLVector3(3, 3, 3));
				bullet->GetRenderObject()->SetColour(playerID == 1 ? Debug::RED : Debug::BLUE);
				bullet->GetRenderObject()->SetDefaultColour(playerID == 1 ? Debug::RED : Debug::BLUE);
				fireTime = FIRE_TIMER;
			}
			else {
				Ray ray(pos, direction);
				RayCollision closestCollision;

				if (physics->Raycast(ray, closestCollision)) {
					selectionObject = (GameObject*)closestCollision.node;
					NCLVector3 normal = closestCollision.collisionNormal;
					GameObject* newPaint = AddPaintPlacingCube(closestCollision.collidedAt, Quaternion(), playerID);
					newPaint->GetTransform().SetOrientation(Quaternion::FromToRotation(NCLVector3(0, 1, 0), normal) * newPaint->GetTransform().GetOrientation());
				
					if (newPaint->GetTransform().GetPosition().x <= meshMap->maxX && newPaint->GetTransform().GetPosition().x >= meshMap->minX
						&& newPaint->GetTransform().GetPosition().z >= meshMap->minZ && newPaint->GetTransform().GetPosition().z <= meshMap->maxZ
						&& newPaint->GetTransform().GetPosition().y >= meshMap->minY && newPaint->GetTransform().GetPosition().y <= meshMap->maxY) {
						testPaint->emplace_back(newPaint);
					}
					if (newPaint->GetTransform().GetPosition().x <= meshMap02->maxX && newPaint->GetTransform().GetPosition().x >= meshMap02->minX
						&& newPaint->GetTransform().GetPosition().z >= meshMap02->minZ && newPaint->GetTransform().GetPosition().z <= meshMap02->maxZ
						&& newPaint->GetTransform().GetPosition().y >= meshMap02->minY && newPaint->GetTransform().GetPosition().y <= meshMap02->maxY) {
						testPaint02->emplace_back(newPaint);
					}
					/*paint.push_back(newPaint);*/

					if (playerID == 1) {
						redPaint++;
					}
					else {
						bluePaint++;
					}
				}
			}
		}
	}
}

void CourseworkGame::FirePaintNet(NCLVector3 direction)
{
	float heightPosOffset = 0.5f;
	NCLVector3 pos;
	NCLMatrix4 view;

	Ray ray2 = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

	NCLVector3 pos1;
	//NCLVector3 pos2;

	pos = players[static_cast<std::vector<NCL::CSC8503::GameObject*, std::allocator<NCL::CSC8503::GameObject*>>::size_type>(1) - nPlayerID]->GetTransform().GetPosition();
	pos = NCLVector3(pos.x, pos.y + heightPosOffset, pos.z);

	//direction = ray2.GetDirection();
	view = world->GetMainCamera()->BuildViewMatrix();


	GameObject* bullet = AddSphereToWorld(pos, 0.5f);
	bullet->GetRigidBody()->SetCollisionFilters(CollisionLayers::Paint, CollisionLayers::Default);
	bullet->SetOnCollisionBeingFunc([=](GameObject* otherActor, ContactPoint contact)->void
		{
			int playerID = (bulletcolour == Debug::BLUE) ? 1 : 0;
			GameObject* newPaint = AddPaintPlacingCube(contact.localA, Quaternion(), playerID);
			newPaint->GetTransform().SetOrientation(Quaternion::FromToRotation(NCLVector3(0, 1, 0), contact.normal) * newPaint->GetTransform().GetOrientation());
			if (newPaint->GetTransform().GetPosition().x <= meshMap->maxX && newPaint->GetTransform().GetPosition().x >= meshMap->minX
				&& newPaint->GetTransform().GetPosition().z >= meshMap->minZ && newPaint->GetTransform().GetPosition().z <= meshMap->maxZ
				&& newPaint->GetTransform().GetPosition().y >= meshMap->minY && newPaint->GetTransform().GetPosition().y <= meshMap->maxY) {
				testPaint->emplace_back(newPaint);
			}
			if (newPaint->GetTransform().GetPosition().x <= meshMap02->maxX && newPaint->GetTransform().GetPosition().x >= meshMap02->minX
				&& newPaint->GetTransform().GetPosition().z >= meshMap02->minZ && newPaint->GetTransform().GetPosition().z <= meshMap02->maxZ
				&& newPaint->GetTransform().GetPosition().y >= meshMap02->minY && newPaint->GetTransform().GetPosition().y <= meshMap02->maxY) {
				testPaint02->emplace_back(newPaint);
			}

			(1-nPlayerID) == 0 ? redPaint++ : bluePaint++;
			bullet->SetToDelete(true);
		}
	);
	NCLVector4 colour = (bulletcolour == Debug::RED) ? Debug::BLUE: Debug::RED;
	bullet->GetRigidBody()->ApplyLinearImpulse(direction * NCLVector3(3, 3, 3));
	bullet->GetRenderObject()->SetColour(colour);
	bullet->GetRenderObject()->SetDefaultColour(colour);
	fireTime = FIRE_TIMER;
	players[1 - nPlayerID]->GetRenderObject()->SetAnimation(animList[1]);
	players[1 - nPlayerID]->setFiring();

}


void CourseworkGame::UpdatePaintAndCleaner() {
	//Clean  and Impulse function
	world->OperateOnContents([&](GameObject* otherActor)->void
		{
			//Clean function
			if (otherActor->GetName() == "Paint splat") {
				otherActor->SetDistanceToCleaner((AIcleaner->GetTransform().GetPosition()
					- otherActor->GetTransform().GetPosition()).LengthSquared());
				if (otherActor->GetDistanceToCleaner() <= 50.0f) {
					if (otherActor->GetRenderObject()->GetColour() == NCLVector4(1, 0, 0, 1.0f)) {
						redPaint--;
					}
					else if (otherActor->GetRenderObject()->GetColour() == NCLVector4(0, 0, 1, 1.0f)) {
						bluePaint--;
					}
					//otherActor->SetToDelete(true);
					world->RemoveGameObject(otherActor);
				}
			}

			//Impulse function
			if (otherActor->GetName() == "Player") {
				otherActor->SetDistanceToCleaner((AIcleaner->GetTransform().GetPosition()
					- otherActor->GetTransform().GetPosition()).LengthSquared());
				if (otherActor->GetDistanceToCleaner() <= 40.0f) {
					NCLVector3 movDir = (otherActor->GetTransform().GetPosition() -
						AIcleaner->GetTransform().GetPosition()).Normalised();
					//otherActor->GetRigidBody()->SetLinearVelocity(movDir * NCLVector3(50.0f, 20.0f, 50.0f));
					otherActor->GetRigidBody()->SetLinearVelocity(movDir * NCLVector3(40.0f, 20.0f, 40.0f));
				}
			}
		}
	);
}

void CourseworkGame::UpdatePaintAndCleaner02() {
	//Clean  and Impulse function
	world->OperateOnContents([&](GameObject* otherActor)->void
		{
			//Clean function
			if (otherActor->GetName() == "Paint splat") {
				otherActor->SetDistanceToCleaner02((AIcleaner02->GetTransform().GetPosition()
					- otherActor->GetTransform().GetPosition()).LengthSquared());
				if (otherActor->GetDistanceToCleaner02() <= 50.0f) {
					if (otherActor->GetRenderObject()->GetColour() == NCLVector4(1, 0, 0, 1.0f)) {
						redPaint--;
					}
					else if (otherActor->GetRenderObject()->GetColour() == NCLVector4(0, 0, 1, 1.0f)) {
						bluePaint--;
					}
					/*otherActor->SetToDelete(true);*/
					world->RemoveGameObject(otherActor);
				}
			}

			//Impulse function
			if (otherActor->GetName() == "Player") {
				otherActor->SetDistanceToCleaner02((AIcleaner02->GetTransform().GetPosition()
					- otherActor->GetTransform().GetPosition()).LengthSquared());
				if (otherActor->GetDistanceToCleaner02() <= 40.0f) {
					NCLVector3 movDir = (otherActor->GetTransform().GetPosition() -
						AIcleaner02->GetTransform().GetPosition()).Normalised();
					//otherActor->GetRigidBody()->SetLinearVelocity(movDir * NCLVector3(50.0f, 20.0f, 50.0f));
					otherActor->GetRigidBody()->SetLinearVelocity(movDir * NCLVector3(40.0f, 20.0f, 40.0f));
				}
			}
		}
	);
}
#endif _x64
