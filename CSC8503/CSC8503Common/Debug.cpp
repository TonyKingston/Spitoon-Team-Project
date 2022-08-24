#include "Debug.h"
#include "../../Common/NCLMatrix4.h"
using namespace NCL;
#ifdef _x64
OGLRenderer* Debug::renderer = nullptr;

std::vector<Debug::DebugStringEntry>	Debug::stringEntries;
std::vector<Debug::DebugLineEntry>		Debug::lineEntries;

const NCLVector4 Debug::RED	= NCLVector4(1, 0, 0, 1);
const NCLVector4 Debug::GREEN	= NCLVector4(0, 1, 0, 1);
const NCLVector4 Debug::BLUE	= NCLVector4(0, 0, 1, 1);

const NCLVector4 Debug::BLACK	= NCLVector4(0, 0, 0, 1);
const NCLVector4 Debug::WHITE	= NCLVector4(1, 1, 1, 1);

const NCLVector4 Debug::YELLOW		= NCLVector4(1, 1, 0, 1);
const NCLVector4 Debug::MAGENTA	= NCLVector4(1, 0, 1, 1);
const NCLVector4 Debug::CYAN		= NCLVector4(0, 1, 1, 1);


void Debug::Print(const std::string& text, const NCLVector2&pos, const NCLVector4& colour) {
	DebugStringEntry newEntry;

	newEntry.data		= text;
	newEntry.position	= pos;
	newEntry.colour		= colour;

	stringEntries.emplace_back(newEntry);
}

void Debug::DrawLine(const NCLVector3& startpoint, const NCLVector3& endpoint, const NCLVector4& colour, float time) {
	DebugLineEntry newEntry;

	newEntry.start	= startpoint;
	newEntry.end	= endpoint;
	newEntry.colour = colour;
	newEntry.time	= time;

	lineEntries.emplace_back(newEntry);
}

void Debug::DrawAxisLines(const NCLMatrix4& modelMatrix, float scaleBoost, float time) {
	NCLMatrix4 local = modelMatrix;
	local.SetPositionNCLVector({0, 0, 0});

	NCLVector3 fwd		= local * NCLVector4(0, 0, -1, 1.0f);
	NCLVector3 up		= local * NCLVector4(0, 1, 0, 1.0f);
	NCLVector3 right	= local * NCLVector4(1, 0, 0, 1.0f);

	NCLVector3 worldPos = modelMatrix.GetPositionNCLVector();

	DrawLine(worldPos, worldPos + (right * scaleBoost)	, Debug::RED, time);
	DrawLine(worldPos, worldPos + (up * scaleBoost)		, Debug::GREEN, time);
	DrawLine(worldPos, worldPos + (fwd * scaleBoost)	, Debug::BLUE, time);
}


void Debug::FlushRenderables(float dt) {
	if (!renderer) {
		return;
	}
	for (const auto& i : stringEntries) {
		renderer->DrawString(i.data, i.position);
	}
	int trim = 0;
	for (int i = 0; i < lineEntries.size(); ) {
		DebugLineEntry* e = &lineEntries[i]; 
		renderer->DrawLine(e->start, e->end, e->colour);
		e->time -= dt;
		if (e->time < 0) {			
			trim++;				
			lineEntries[i] = lineEntries[lineEntries.size() - trim];
		}
		else {
			++i;
		}		
		if (i + trim >= lineEntries.size()) {
			break;
		}
	}
	lineEntries.resize(lineEntries.size() - trim);

	stringEntries.clear();
}
#endif _x64