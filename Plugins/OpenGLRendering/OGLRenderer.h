/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "../../Common/RendererBase.h"

#include "../../Common/NCLVector3.h"
#include "../../Common/NCLVector4.h"


#ifdef _WIN32
#include "windows.h"
#endif

#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif


#include <string>
#include <vector>

namespace NCL {
	class MeshGeometry;
	class MeshAnimation;

	namespace Maths {
		class NCLMatrix4;
	}

	namespace Rendering {
		class ShaderBase;
		class TextureBase;

		class OGLMesh;
		class OGLShader;

		class SimpleFont;
		
		class OGLRenderer : public RendererBase
		{
		public:
			friend class OGLRenderer;
			OGLRenderer(Window& w);
			~OGLRenderer();

			void OnWindowResize(int w, int h)	override;
			bool HasInitialised()				const override {
				return initState;
			}

			void ForceValidDebugState(bool newState) {
				forceValidDebugState = newState;
			}

			virtual bool SetVerticalSync(VerticalSyncState s);

			void DrawString(const std::string& text, const NCLVector2&pos, const NCLVector4& colour = NCLVector4(0.75f, 0.75f, 0.75f,1), float size = 20.0f );
			void DrawLine(const NCLVector3& start, const NCLVector3& end, const NCLVector4& colour);

			virtual NCLMatrix4 SetupDebugLineMatrix()	const;
			virtual NCLMatrix4 SetupDebugStringMatrix()const;

		protected:			
			void BeginFrame()	override;
			void RenderFrame()	override;
			void EndFrame()		override;
			void SwapBuffers()  override;

			void DrawDebugData(bool isSplit = false);
			void DrawDebugStrings(bool isSplit = false);
			void DrawDebugLines(bool isSplit = false);

			void BindShader(ShaderBase*s);
			void BindTextureToShader(const TextureBase*t, const std::string& uniform, int texUnit) const;
			void BindMesh(MeshGeometry*m);
			void BindAnimation(MeshAnimation* a);
			void DrawBoundMesh(int subLayer = 0, int numInstances = 1);
#ifdef _WIN32
			void InitWithWin32(Window& w);
			void DestroyWithWin32();
			HDC		deviceContext;		//...Device context?
			HGLRC	renderContext;		//Permanent Rendering Context		
#endif
		private:
			struct DebugString {
				Maths::NCLVector4 colour;
				Maths::NCLVector2	pos;
				float			size;
				std::string		text;
			};

			struct DebugLine {
				Maths::NCLVector3 start;
				Maths::NCLVector3 end;
				Maths::NCLVector4 colour;
			};

			OGLMesh* debugLinesMesh;
			OGLMesh* debugTextMesh;

			OGLMesh*	boundMesh;
			OGLShader*	boundShader;

			OGLShader*  debugShader;
			SimpleFont* font;
			std::vector<DebugString>	debugStrings;
			std::vector<DebugLine>		debugLines;

			bool initState;
			bool forceValidDebugState;
		};
	}
}