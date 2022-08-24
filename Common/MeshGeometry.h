#pragma once
#include <vector>
#include "NCLVector2.h"
#include "NCLVector3.h"
#include "NCLVector4.h"
using std::vector;

namespace NCL {
	namespace Maths {
		class NCLVector2;
		class NCLVector3;
		class NCLVector4;
		class NCLMatrix4;
	}
	namespace Rendering {
		class RendererBase;
	}
	using namespace Maths;

	enum GeometryPrimitive {
		Points,
		Lines,
		Triangles,
		TriangleFan,
		TriangleStrip,
		Patches
	};

	enum VertexAttribute {
		Positions,
		Colours,
		TextureCoords,
		Normals,
		Tangents,
		JointWeights,
		JointIndices,
		MAX_ATTRIBUTES
	};

	struct SubMesh {
		int start;
		int count;
	};

	class MeshGeometry
	{
	public:		
		virtual ~MeshGeometry();

		GeometryPrimitive GetPrimitiveType() const {
			return primType;
		}

		void SetPrimitiveType(GeometryPrimitive type) {
			primType = type;
		}

		unsigned int GetVertexCount() const {
			return (unsigned int)positions.size();
		}

		unsigned int GetIndexCount()  const {
			return (unsigned int)indices.size();
		}

		unsigned int GetJointCount() const {
			return (unsigned int)jointNames.size();
		}

		unsigned int GetSubMeshCount() const {
			return (unsigned int)subMeshes.size();
		}

		const SubMesh* GetSubMesh(unsigned int i) const {
			if (i > subMeshes.size()) {
				return nullptr;
			}
			return &subMeshes[i];
		}

		int GetIndexForJoint(const std::string &name) const;

		const vector<NCLMatrix4>& GetBindPose() const {
			return bindPose;
		}
		const vector<NCLMatrix4>& GetInverseBindPose() const {
			return inverseBindPose;
		}

		void SetJointNames(std::vector < std::string > & newnames);

		bool GetTriangle(unsigned int i, NCLVector3& a, NCLVector3& b, NCLVector3& c) const;
		bool GetNormalForTri(unsigned int i, NCLVector3& n) const;
		bool HasTriangle(unsigned int i) const;

		const vector<NCLVector3>&		GetPositionData()		const { return positions;	}
		const vector<NCLVector2>&		GetTextureCoordData()	const { return texCoords;	}
		const vector<NCLVector4>&		GetColourData()			const { return colours;		}
		const vector<NCLVector3>&		GetNormalData()			const { return normals;		}
		const vector<NCLVector4>&		GetTangentData()		const { return tangents;	}


		const vector<NCLVector4>& GetSkinWeightData()		const { return skinWeights; }
		const vector<NCLVector4>& GetSkinIndexData()		const { return skinIndices; }

		const vector<int>& GetJointParents()	const {
			return jointParents;
		}


		const vector<unsigned int>& GetIndexData()			const { return indices;		}


		void SetVertexPositions(const vector<NCLVector3>& newVerts);
		void SetVertexTextureCoords(const vector<NCLVector2>& newTex);

		void SetVertexColours(const vector<NCLVector4>& newColours);
		void SetVertexNormals(const vector<NCLVector3>& newNorms);
		void SetVertexTangents(const vector<NCLVector4>& newTans);
		void SetVertexIndices(const vector<unsigned int>& newIndices);

		void SetVertexSkinWeights(const vector<NCLVector4>& newSkinWeights);
		void SetVertexSkinIndices(const vector<NCLVector4>& newSkinIndices);


		void	TransformVertices(const NCLMatrix4& byNCLMatrix);

		void RecalculateNormals();
		void RecalculateTangents();

		void SetDebugName(const std::string& debugName);

		virtual void UploadToGPU(Rendering::RendererBase* renderer = nullptr) = 0;

		static MeshGeometry* GenerateTriangle(MeshGeometry* input);

	protected:
		MeshGeometry();
		MeshGeometry(const std::string&filename);

		void ReadRigPose(std::ifstream& file, vector<NCLMatrix4>& into);
		void ReadJointParents(std::ifstream& file);
		void ReadJointNames(std::ifstream& file);
		void ReadSubMeshes(std::ifstream& file, int count);
		void ReadSubMeshNames(std::ifstream& file, int count);

		bool	GetVertexIndicesForTri(unsigned int i, unsigned int& a, unsigned int& b, unsigned int& c) const;

		virtual bool ValidateMeshData();

		std::string			debugName; //used when an APU allows setting debug tags
		GeometryPrimitive	primType;
		vector<NCLVector3>		positions;

		vector<NCLVector2>			texCoords;
		vector<NCLVector4>			colours;
		vector<NCLVector3>			normals;
		vector<NCLVector4>			tangents;
		vector<unsigned int>	indices;

		vector<SubMesh>			subMeshes;
		vector<std::string>		subMeshNames;

		//Allows us to have 4 weight skinning 
		vector<NCLVector4>		skinWeights;
		vector<NCLVector4>		skinIndices;
		vector<std::string>	jointNames;
		vector<int>			jointParents;

		vector<NCLMatrix4>		bindPose;
		vector<NCLMatrix4>		inverseBindPose;
	};
}