#pragma once
#include "Client_Net_Common.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/BehaviourAction.h"
#include "../CSC8503Common/BehaviourSequence.h"
#include "../CSC8503Common/BehaviourSelector.h"
#include "../CSC8503Common/Physics/PxSystem.h"
#include <unordered_map>
#ifdef _WIN64
#include "../../Plugins/OpenGLRendering/OGLResourceManager.h"
#include "GameTechRenderer.h"

#endif _WIN64
#include "../CSC8503Common/SystemConfig.h"
#include "../CSC8503Common/NavigationMap.h"
#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/NavigationMesh.h"
#include "CharacterController.h"
#include "../CSC8503Common/Cleaner.h"
#include <memory>
#include "Audio.h"

#include "Client_Net_Common.h"

#ifdef _WIN64
namespace NCL {
	namespace CSC8503 {
		class D_GUI;
#ifdef _x64
		class CourseworkGame : NCL::net::client_interface<GameMsg> {
#else
		class CourseworkGame {
#endif
		public:
			CourseworkGame(Window* w);
			~CourseworkGame();

			virtual void UpdateGame(float dt);
			void UpdateStart() { renderer->RenderStartView(); }

			// Getting Game Level
			int GetCurrentLevel() { return currentLevel; }
			void LoadLevel(int i = 0);
			void LoadConvexMeshTest();
			void LoadPlayer();

			D_GUI* GetUI() const { return gameUI; }

			ResourceManager* GetResourceManager() {
				return resourceManager;
			}

			ShaderBase* GetBasicShader() {
				return basicShader;
			}

			PxSystem* GetPhysics() {
				return physics;
			}

			GameWorld* GetWorld() {
				return world;
			}
#ifdef _WIN64
			GameTechRenderer* GetRenderer() {
				return renderer;
			}
#endif _WIN64

			bool GetDebugMode() {
				return debug_mode;
			}

			void SetDebugMode(bool isDebug) {
				debug_mode = isDebug;
			}

			bool GetChangeColor() {
				return changeColour;
			}

			void SetFreeCamera(bool FreeCameraTrigger) {
				isFreeCamera = FreeCameraTrigger;
			}

			bool GetFreeCamerea() {
				return isFreeCamera;
			}

			void SetChangeColor(bool isChangeColor) {
				changeColour = isChangeColor;
			}

			Cleaner* GetCleaner() { return AIcleaner; }
			Cleaner* GetCleaner02() { return AIcleaner02; }

			void InitCamera();
			void InitView() { world->GetMainCamera()->InitView(players[0]->GetTransform().GetPosition()); }

			const int& GetRedScore() { return redPaint; }
			const int& GetBlueScore() { return bluePaint; }
		protected:
			friend class DebugMenu;
			friend class PauseMenu;

			// Original Game Tech methods
			void InitialiseAssets();
			void UpdateKeys();
			bool SelectObject();
			void PlayerMovement(GameObject *playerSphere, int playerNum);
			void LockedObjectMovement();
			void DebugObjectMovement();
			void MoveSelectedObject();
			void LoadEvilGeniusScene(string filename, const NCLVector3& position, NCLVector3 dimensions, Maths::NCLVector3 rotation);
			void InitialiseAnimations();

			void UpdateCamera(const float &dt);

			// Generating Environment
			
			GameObject* AddFloorToWorld(const NCLVector3& position, const NCLVector3& floorSize, const NCLVector4& colour = NCLVector4(1, 1, 1, 1), bool isSpring = false);
			GameObject* AddSphereToWorld(const NCLVector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const NCLVector3& position, NCLVector3 dimensions, float inverseMass = 10.0f, bool isSpring = false, bool isStatic = false, bool isAItest = false);
			GameObject* AddCapsuleToWorld(const NCLVector3& position, float halfHeight, float radius, float inverseMass = 10.0f, bool isSpring = false);
			PlayerObject* AddPlayerToWorld(const NCLVector3& position, float halfHeight, float radius, float inverseMass = 10.0f, bool isSpring = false);
			GameObject* AddPowerupToWorld(const NCLVector3& position, string name);
			GameObject* AddKillPlaneToWorld(const NCLVector3& position);
			GameObject* AddGun(const NCLVector3& position);
			Cleaner* AddCleanerToWorld(const NCLVector3& position);


			// UI / Menu
			//void MainMenu(const std::string& title = "", const NCLVector4& colour = NCLVector4(1, 1, 1, 1));
			int currentLevel = 0; // Game initialises with to level

			// AI / Pathfinding
			void InitNavMeshAssets();
			NavigationMesh* meshMap;
			NavigationMesh* meshMap02;
			std::vector<GameObject*>* testPaint;
			std::vector<GameObject*>* testPaint02;
			Cleaner* CourseworkGame::AddCleaner02ToWorld(const NCLVector3& position);
			

			vector<PlayerObject*> players;

			CharacterController* playerControllers[2];
			GameObject* enemySphere;
			Cleaner* AIcleaner;
			Cleaner* AIcleaner02;
			void UpdatePaintAndCleaner();
			void UpdatePaintAndCleaner02();

			const float PAINT_DISTANCE = 100.0f; // squared distance between paint and cleaner for paint to be removed
			const float CLEANER_DISTANCE = 64.0f; // squared distance between player and cleaner for player to be pushed back



			// JsonTest
			GameObject* JsonObject;
			MeshGeometry* JsonMesh;
			MeshMaterial* JsonMat;

			//UI
			D_GUI *gameUI;

			// Game variables
#ifdef _x64
			GameTechRenderer* renderer;
#else
			RendererBase* renderer;
#endif _x64
			PxSystem* physics;
			GameWorld* world;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			GameObject* forwardObject = nullptr; // Tutorial 1 Q.2

			TextureBase* basicTex = nullptr;
			TextureBase* gunTex = nullptr;
			ShaderBase* basicShader = nullptr;
			ShaderBase* playerShader = nullptr;
			// Coursework Additional Functionality
			GameObject* lockedObject = nullptr;
			NCLVector3 lockedOffset = NCLVector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			bool inSelectionMode = false;

			// Config
			SystemConfig* config;

			//Models and Animations
			vector<TextureBase*> guardMat;
			MeshAnimation* animList[6];

			// Alpha paint tech
			GameObject* AddPaintToWorld(const NCLVector3& position, const Quaternion& orientation, int playerID);
			void FirePaint(int playerID);
			void FirePaintNet(NCLVector3 direction);
			/*std::vector<GameObject*> paint;*/
			int redPaint;
			int bluePaint;
			const float FIRE_TIMER = 0.5f;
			float fireTime = FIRE_TIMER;

			// Actual game paint tech
			GameObject* AddPaintPlacingCube(const NCLVector3& position, const Quaternion& orientation, int playerID);

		private:
			Window* window;
			std::unordered_map<uint32_t, sPlayerDescription> mapObjects;
			uint32_t nPlayerID = 0;
			sPlayerDescription descPlayer;

			int current_players[2];
			int playernum;
			int numplayers = 0;
			bool bWaitingForConnection = true;

			ResourceManager* resourceManager;

			bool debug_mode = false;
			void ShowRigidBodyStatus();
			bool changeColour= false; // For setting rigid body colour
			bool isFreeCamera = false;
		public:
			bool playerAssigned[2];
			bool start = true;
			bool onUserCreate();
			bool onUserUpdate();
			bool startReady();
			bool isNetworked;
			vector<NCLVector3> reservoir;
			NCLVector3 currentpaint;
			NCLVector3 previouspaint = NCLVector3(0, 0, 0);
			NCLVector4 bulletcolour;
		};

	}
}
#endif _WIN64