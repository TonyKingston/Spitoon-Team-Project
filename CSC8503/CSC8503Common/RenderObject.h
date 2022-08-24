#pragma once
#include "../../Common/NCLMatrix4.h"
#include "../../Common/TextureBase.h"
#include "../../Common/ShaderBase.h"
#include "../../Common/NCLVector4.h"
#include "../../Common/MeshMaterial.h"
#include "../../Common/MeshAnimation.h"
#include "../../Common/ResourceManager.h"
#include <vector>

namespace NCL {
	using namespace NCL::Rendering;
	using std::vector;
	class MeshGeometry;
	namespace CSC8503 {
		class Transform;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader, Maths::NCLVector4 colour = Maths::NCLVector4(1, 1, 1, 1));
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, vector<TextureBase*> textures, ShaderBase* shader, Maths::NCLVector4 colour = Maths::NCLVector4(1, 1, 1, 1));
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, string material, ShaderBase* shader, Maths::NCLVector4 colour = Maths::NCLVector4(1,1,1,1));
			~RenderObject();

			static void InitManager(ResourceManager* resourceManager) {
				manager = resourceManager;
			}
			void Update(float dt)
			{
				if (animation) {
					frameTime -= dt;
					while (frameTime < 0.0f)
					{
						for (int i = 0; i < 1; i++)
						{
							currentFrame = (currentFrame + 1) % animation->GetFrameCount();
							frameTime += 1.0f / animation->GetFrameRate();
						}
					}
				}
			}

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture;
			}

			TextureBase* GetBumpTexture() const {
				if (textures.size() > 1) {
					return textures[1];
				}
				return nullptr;
			}

			vector<TextureBase*> GetTextures() const {
				return textures;
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}

			MeshAnimation* GetAnimation() const {
				return animation;
			}

			void SetAnimation(MeshAnimation* anim) {
				animation = anim;
			}

			void SetAnimationList(MeshAnimation** anim) {
				animList = anim;
			}

			int GetCurrentFrame() const {
				return currentFrame;
			}

			void ResetCurrentFrame() {
				currentFrame = 0;
			}
 
			Transform*		GetTransform() const {
				return transform;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}

			void SetColour(const Maths::NCLVector4& c) {
				colour = c;
			}

			Maths::NCLVector4 GetColour() const {
				return colour;
			}

			void AddChild(RenderObject* o) {
				o->parent = this;
				children.push_back(o);
			}

			vector<RenderObject*> GetChildren() {
				return children;
			}

			std::vector < RenderObject* >::const_iterator GetChildIteratorStart() {
				return children.begin();
			}

			std::vector < RenderObject* >::const_iterator GetChildIteratorEnd() {
				return children.end();
			}

			float GetBoundingRadius() const { return boundingRadius; }
			void SetBoundingRadius(float f) { boundingRadius = f; }

			float GetCameraDistance() const { return distanceFromCamera; }
			void SetCameraDistance(float f) { distanceFromCamera = f; }

			static bool CompareByCameraDistance(RenderObject* a, RenderObject* b) {
				return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
			}
			void setAnimationIndex(int ats)
			{
				currentAnim = ats;
			}

			MeshAnimation** animList;


			//Default colour
			NCLVector4 GetDefaultColour() {
				return dColour;
			}

			void SetDefaultColour(NCLVector4 defaultColour) {
				dColour = defaultColour;
			}


		protected:
			MeshGeometry*	mesh;
			TextureBase*	texture;
			vector<TextureBase*> textures;
			vector<TextureBase*> diffuseTextures;
			vector<TextureBase*> bumpTextures;
			ShaderBase*		shader;
			Transform*		transform;
			Maths::NCLVector4			colour;
			Maths::NCLVector4          dColour = NCLVector4();

			
			MeshAnimation*  animation;
			int             currentFrame;
			int				currentAnim;

			RenderObject* parent;
			vector<RenderObject*> children;

			float distanceFromCamera;
			// TODO: Fix frustum culling for both cameras
			float boundingRadius = 200;

			static ResourceManager* manager;
			float frameTime;
		};
	}
}
