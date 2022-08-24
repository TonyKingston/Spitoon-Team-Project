#pragma once
#ifdef _x64
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include <vector>
#include <string>

namespace NCL {
	class Debug
	{
	public:
		static void Print(const std::string& text, const NCLVector2&pos, const NCLVector4& colour = NCLVector4(1, 1, 1, 1));
		static void DrawLine(const NCLVector3& startpoint, const NCLVector3& endpoint, const NCLVector4& colour = NCLVector4(1, 1, 1, 1), float time = 0.0f);

		static void DrawAxisLines(const NCLMatrix4 &modelMatrix, float scaleBoost = 1.0f, float time = 0.0f);

		static void SetRenderer(OGLRenderer* r) {
			renderer = r;
		}

		static void FlushRenderables(float dt);


		static const NCLVector4 RED;
		static const NCLVector4 GREEN;
		static const NCLVector4 BLUE;

		static const NCLVector4 BLACK;
		static const NCLVector4 WHITE;

		static const NCLVector4 YELLOW;
		static const NCLVector4 MAGENTA;
		static const NCLVector4 CYAN;

	protected:
		struct DebugStringEntry {
			std::string	data;
			NCLVector2 position;
			NCLVector4 colour;
		};

		struct DebugLineEntry {
			NCLVector3 start;
			NCLVector3 end;
			float	time;
			NCLVector4 colour;
		};

		Debug() {}
		~Debug() {}

		static std::vector<DebugStringEntry>	stringEntries;
		static std::vector<DebugLineEntry>	lineEntries;

		static OGLRenderer* renderer;
	};
}
#endif _x64