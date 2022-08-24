#include "MeshGeometry.h"
#include "Assets.h"
#include "NCLVector2.h"
#include "NCLVector3.h"
#include "NCLVector4.h"
#include "NCLMatrix4.h"

#include <fstream>
#include <string>

using namespace NCL;
using namespace Maths;

MeshGeometry::MeshGeometry()
{
	primType	= GeometryPrimitive::Triangles;
}

enum class GeometryChunkTypes {
	VPositions		= 1 << 0,
	VNormals		= 1 << 1,
	VTangents		= 1 << 2,
	VColors			= 1 << 3,
	VTex0			= 1 << 4,
	VTex1			= 1 << 5,
	VWeightValues	= 1 << 6,
	VWeightIndices	= 1 << 7,
	Indices			= 1 << 8,
	JointNames		= 1 << 9,
	JointParents	= 1 << 10,
	BindPose		= 1 << 11,
	BindPoseInv		= 1 << 12,
	Material		= 1 << 13,
	SubMeshes		= 1 << 14,
	SubMeshNames	= 1 << 15
};

enum class GeometryChunkData {
	dFloat, //Just float data
	dShort, //Translate from -32k to 32k to a float
	dByte,	//Translate from -128 to 127 to a float
};

void* ReadVertexData(GeometryChunkData dataType, GeometryChunkTypes chunkType, int numVertices) {
	int numElements = 3;

	if (chunkType == GeometryChunkTypes::VTex0 ||
		chunkType == GeometryChunkTypes::VTex1) {
		numElements = 2;
	}
	else if (chunkType == GeometryChunkTypes::VColors) {
		numElements = 4;
	}

	int bytesPerElement = 4;

	if (dataType == GeometryChunkData::dShort) {
		bytesPerElement = 2;
	}
	else if (dataType == GeometryChunkData::dByte) {
		bytesPerElement = 1;
	}

	char* data = new char[numElements * bytesPerElement];

	return data;
}

void ReadTextFloats(std::ifstream& file, vector<NCLVector2>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		NCLVector2 temp;
		file >> temp.x;
		file >> temp.y;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, vector<NCLVector3>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		NCLVector3 temp;
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, vector<NCLVector4>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		NCLVector4 temp;
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		file >> temp.w;
		element.emplace_back(temp);
	}
}

void ReadIndices(std::ifstream& file, vector<unsigned int>& elements, int numIndices) {
	for (int i = 0; i < numIndices; ++i) {
		unsigned int temp;
		file >> temp;
		elements.emplace_back(temp);
	}
}

MeshGeometry::MeshGeometry(const std::string&filename) {
	primType = GeometryPrimitive::Triangles;
	std::ifstream file(Assets::MESHDIR + filename);

	std::string filetype;
	int fileVersion;

	file >> filetype;

	if (filetype != "MeshGeometry") {
		std::cout << "File is not a MeshGeometry file!" << std::endl;
		return;
	}

	file >> fileVersion;

	if (fileVersion != 1) {
		std::cout << "MeshGeometry file has incompatible version!" << std::endl;
		return;
	}

	int numMeshes	= 0; //read
	int numVertices = 0; //read
	int numIndices	= 0; //read
	int numChunks   = 0; //read

	file >> numMeshes;
	file >> numVertices;
	file >> numIndices;
	file >> numChunks;
	
	for (int i = 0; i < numChunks; ++i) {
		int chunkType = (int)GeometryChunkTypes::VPositions;

		file >> chunkType;

		switch ((GeometryChunkTypes)chunkType) {
			case GeometryChunkTypes::VPositions:ReadTextFloats(file, positions, numVertices);	break;
			case GeometryChunkTypes::VColors:	ReadTextFloats(file, colours, numVertices);		break;
			case GeometryChunkTypes::VNormals:	ReadTextFloats(file, normals, numVertices);		break;
			case GeometryChunkTypes::VTangents:	ReadTextFloats(file, tangents, numVertices);	break;
			case GeometryChunkTypes::VTex0:		ReadTextFloats(file, texCoords, numVertices);	break;
			case GeometryChunkTypes::Indices:	ReadIndices(file, indices, numIndices); break;			
				
			case GeometryChunkTypes::VWeightValues:		ReadTextFloats(file, skinWeights, numVertices);  break;
			case GeometryChunkTypes::VWeightIndices:	ReadTextFloats(file, skinIndices, numVertices);  break;
			case GeometryChunkTypes::JointNames:		ReadJointNames(file);		break;
			case GeometryChunkTypes::JointParents:		ReadJointParents(file);		break;
			case GeometryChunkTypes::BindPose:			ReadRigPose(file, bindPose);  break;
			case GeometryChunkTypes::BindPoseInv:		ReadRigPose(file, inverseBindPose);  break;
			case GeometryChunkTypes::SubMeshes: 		ReadSubMeshes(file, numMeshes); break;
			case GeometryChunkTypes::SubMeshNames: 		ReadSubMeshNames(file, numMeshes); break;
		}
	}
}

MeshGeometry::~MeshGeometry()
{
}

bool MeshGeometry::HasTriangle(unsigned int i) const {
	int triCount = 0;
	if (GetIndexCount() > 0) {
		triCount = GetIndexCount() / 3;
	}
	else {
		triCount = GetVertexCount() / 3;
	}
	return i < (unsigned int)triCount;
}


bool	MeshGeometry::GetVertexIndicesForTri(unsigned int i, unsigned int& a, unsigned int& b, unsigned int& c) const {
	if (!HasTriangle(i)) {
		return false;
	}
	if (GetIndexCount() > 0) {
		a = indices[(i * 3)];
		b = indices[(i * 3) + 1];
		c = indices[(i * 3) + 2];
	}
	else {
		a = (i * 3);
		b = (i * 3) + 1;
		c = (i * 3) + 2;
	}
	return true;
}

bool MeshGeometry::GetTriangle(unsigned int i, NCLVector3& va, NCLVector3& vb, NCLVector3& vc) const {
	bool hasTri = false;
	unsigned int a, b, c;
	hasTri = GetVertexIndicesForTri(i, a, b, c);
	if (!hasTri) {
		return false;
	}
	va = positions[a];
	vb = positions[b];
	vc = positions[c];
	return true;
}


bool MeshGeometry::GetNormalForTri(unsigned int i, NCLVector3& n) const {
	NCLVector3 a, b, c;

	bool hasTri = GetTriangle(i, a, b, c);
	if (!hasTri) {
		return false;
	}

	NCLVector3 ba = b - a;
	NCLVector3 ca = c - a;
	n = NCLVector3::Cross(ba, ca);
	n.Normalise();
	return true;
}

void	MeshGeometry::TransformVertices(const NCLMatrix4& byNCLMatrix) {

}

void	MeshGeometry::RecalculateNormals() {

}

void	MeshGeometry::RecalculateTangents() {

}

void MeshGeometry::SetVertexPositions(const vector<NCLVector3>& newVerts) {
	positions = newVerts;
}

void MeshGeometry::SetVertexTextureCoords(const vector<NCLVector2>& newTex) {
	texCoords = newTex;
}

void MeshGeometry::SetVertexColours(const vector<NCLVector4>& newColours) {
	colours = newColours;
}

void MeshGeometry::SetVertexNormals(const vector<NCLVector3>& newNorms) {
	normals = newNorms;
}

void MeshGeometry::SetVertexTangents(const vector<NCLVector4>& newTans) {
	tangents = newTans;
}

void MeshGeometry::SetVertexIndices(const vector<unsigned int>& newIndices) {
	indices = newIndices;
}

void MeshGeometry::SetVertexSkinWeights(const vector<NCLVector4>& newSkinWeights) {
	skinWeights = newSkinWeights;
}

void MeshGeometry::SetVertexSkinIndices(const vector<NCLVector4>& newSkinIndices) {
	skinIndices = newSkinIndices;
}

MeshGeometry* MeshGeometry::GenerateTriangle(MeshGeometry* input) {
	input->SetVertexPositions({NCLVector3(-1,-1,0), NCLVector3(1,-1,0), NCLVector3(0,1,0) });
	input->SetVertexColours({ NCLVector4(1,0,0,1), NCLVector4(0,1,0,1), NCLVector4(0,0,1,1) });
	input->SetVertexTextureCoords({ NCLVector2(0,0), NCLVector2(1,0), NCLVector2(0.5, 1) });
	input->SetVertexIndices({ 0,1,2 });
	input->debugName = "Test-Triangle";
	return input;
}

void MeshGeometry::SetDebugName(const std::string& newName) {
	debugName = newName;
}

int MeshGeometry::GetIndexForJoint(const std::string& name) const {
	for (int i = 0; i < jointNames.size(); ++i) {
		if (jointNames[i] == name) {
			return i;
		}
	}
	return -1;
}

void MeshGeometry::SetJointNames(std::vector < std::string >& newNames) {
	jointNames = newNames;
}

void MeshGeometry::ReadRigPose(std::ifstream& file, vector<NCLMatrix4>& into) {
	int matCount = 0;
	file >> matCount;

	for (int i = 0; i < matCount; ++i) {
		NCLMatrix4 mat;
		for (int i = 0; i < 16; ++i) {
			file >> mat.array[i];
		}
		into.emplace_back(mat);
	}
}

void MeshGeometry::ReadJointParents(std::ifstream& file) {
	int jointCount = 0;
	file >> jointCount;

	for (int i = 0; i < jointCount; ++i) {
		int id = -1;
		file >> id;
		jointParents.emplace_back(id);
	}
}

void MeshGeometry::ReadJointNames(std::ifstream& file) {
	int jointCount = 0;
	file >> jointCount;
	std::string jointName;
	std::getline(file, jointName);

	for (int i = 0; i < jointCount; ++i) {
		std::string jointName;
		std::getline(file, jointName);
		jointNames.emplace_back(jointName);
	}
}

void MeshGeometry::ReadSubMeshes(std::ifstream& file, int count) {
	for (int i = 0; i < count; ++i) {
		SubMesh m;
		file >> m.start;
		file >> m.count;
		subMeshes.emplace_back(m);
	}
}

void MeshGeometry::ReadSubMeshNames(std::ifstream& file, int count) {
	std::string scrap;
	std::getline(file, scrap);

	for (int i = 0; i < count; ++i) {
		std::string meshName;
		std::getline(file, meshName);
		subMeshNames.emplace_back(meshName);
	}
}

bool MeshGeometry::ValidateMeshData() {
	if (GetPositionData().empty()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " does not have any vertex positions!" << std::endl;
		return false;
	}
	if (!GetTextureCoordData().empty() && GetTextureCoordData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect texture coordinate attribute count!" << std::endl;
		return false;
	}
	if (!GetColourData().empty() && GetColourData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect colour attribute count!" << std::endl;
		return false;
	}
	if (!GetNormalData().empty() && GetNormalData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect normal attribute count!" << std::endl;
		return false;
	}
	if (!GetTangentData().empty() && GetTangentData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect tangent attribute count!" << std::endl;
		return false;
	}

	if (!GetSkinWeightData().empty() && GetSkinWeightData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect skin weight attribute count!" << std::endl;
		return false;
	}

	if (!GetSkinIndexData().empty() && GetSkinIndexData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect skin index attribute count!" << std::endl;
		return false;
	}

	return true;
}