//----------------------------------------------------------------------------------------------------------------------------------//
// FBX Loader.h DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

#include <windows.h>

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include <fbxsdk.h>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "VertexType.h"

// Necessary lib files kan be linked here, but also be added by going to:
// Properties->Linker->Input->Additional Dependencies
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace std;

//----------------------------------------------------------------------------------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------------------------------------------------------------------------------//

struct VS_SKINNED_DATA { // Struct to hold the Inverse Global Bind Pose matrices on the GPU

	XMMATRIX gBoneTransform[4];
};

struct VertexBlendInfo {

	unsigned int BlendingIndex;
	double BlendingWeight;

	VertexBlendInfo():

		BlendingIndex(0),
		BlendingWeight(0.0)

	{}

	bool operator < (const VertexBlendInfo& other) {

		return (BlendingWeight > other.BlendingWeight);
	}
};

struct Keyframe { // Stores the attributes of a keyframe in an animation

	FbxLongLong FrameNumber;
	FbxAMatrix GlobalTransform;
	Keyframe* Next;

	Keyframe() :

		Next(nullptr)

	{}

};

struct Joint { // Stores the attributes of a joint node

	const char* Name;
	int ParentIndex;

	FbxAMatrix GlobalBindposeInverse;
	FbxAMatrix TransformMatrix;
	FbxAMatrix TransformLinkMatrix;

	Keyframe* Animation;
	FbxNode* Node;

	Joint() :

		Node(nullptr),
		Animation(nullptr)

	{
		GlobalBindposeInverse.SetIdentity();
		ParentIndex = -1;

	}

};

struct BlendingIndexWeightPair { // Middle hand container to help with passing Vertex/Skinning pair data

	int BlendIndex;
	double BlendWeight;

	BlendingIndexWeightPair() :
		BlendIndex(0),
		BlendWeight(0)
	{}
};

struct ControlPoint { // Resembles a physical vertex point in the FBX SDK

	XMFLOAT3 Position;
	vector<BlendingIndexWeightPair> BlendingInfo;

	ControlPoint() {

		BlendingInfo.reserve(4);
	}

};

struct Skeleton { // Stores every joint in the skeleton hierarchy from the loaded FBX file

	vector<Joint> hierarchy;
	int hierarchyDepth;

};

struct Blend { // Temporary struct containing a VertexBlendInfo vector for debugging purposes

	vector<VertexBlendInfo> VertexBlendingInfoList;

	void SortBlendingInfoByWeight() {

		sort(VertexBlendingInfoList.begin(), VertexBlendingInfoList.end());
	}

	bool operator==(const Blend& other) {

		bool sameBlendingInfo = true;

		// We only have to compare the blending information of another vertex if there is any blending information in that vertex
		if (!(VertexBlendingInfoList.empty() && other.VertexBlendingInfoList.empty())) {

			// Each vertex should only hold 4 index-weight blending info pairs
			for (unsigned int i = 0; i < 4; i++) {

				if (VertexBlendingInfoList[i].BlendingIndex != other.VertexBlendingInfoList[i].BlendingIndex ||
					abs(VertexBlendingInfoList[i].BlendingWeight - other.VertexBlendingInfoList[i].BlendingWeight) > 0.001) {

					sameBlendingInfo = false;
					break;
				}
			}
		}

	}
};

class FbxImport { // Handler Class to store FBX data and manage the Skeletal Animation System

public:

	//----------------------------------------------------------------------------------------------------------------------------------//
	// CONSTRUCTOR AND DESTRUCTOR
	//----------------------------------------------------------------------------------------------------------------------------------//

	FbxImport();
	~FbxImport();

	//----------------------------------------------------------------------------------------------------------------------------------//
	// BUFFERS
	//----------------------------------------------------------------------------------------------------------------------------------//

	ID3D11Buffer* gBoneBuffer;
	ID3D11Buffer* gBoneVertexBuffer;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// PRIMARY FUNCTIONS AND VARIABLES
	//----------------------------------------------------------------------------------------------------------------------------------//

	// Responsible for loading in and initializing all data of interest for us
	// Returns the output data for the vertex class used in the vertex buffer
	HRESULT LoadFBX(vector<Vertex_Bone>* pOutVertexVector);

	XMMATRIX Load4X4JointTransformations(Joint joint, int transformIndex);
	XMMATRIX Load4X4Transformations(FbxAMatrix fbxMatrix);
	void UpdateAnimation(VS_SKINNED_DATA* boneBufferPointer, float dt);
	
	Skeleton meshSkeleton;
	int frameIndex;

	XMMATRIX localTransform[18];	// Bind pose matrix

	vector<Keyframe*> offset;
	vector<Keyframe*> offsetStart;
	vector<Vertex_Bone>vertices;	// Extra copy of vertices

private:

	// Separate function to load only mesh data and store control points
	void CreateVertexData(FbxNode* rootNode, vector<Vertex_Bone>* pOutVertexVector);

	// Separate function to load only skeletal data
	void LoadSkeletonHierarchy(FbxNode* rootNode);

	// Function used in LoadSkeletonHierarchy function to perform a deep first search to store the joints in correct order
	void RecursiveDepthFirstSearch(FbxNode* node, int depth, int index, int parentIndex);

	// Function to process the animation data when the skeleton hierarchy has been loaded
	void GatherAnimationData(FbxNode* node, FbxScene* scene);

	void SetGlobalTransform();

	//----------------------------------------------------------------------------------------------------------------------------------//
	// SECONDARY FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//

	void InitializeAnimation();

	// Geometric transform must be taken into account, even though it's often just an identity matrix (especially in Maya)
	FbxAMatrix GetGeometryTransformation(FbxNode* node);

	// Function used to find the index of joint by giving the function the name of the current joint being processed
	unsigned int FindJointIndexByName(std::string& jointName, Skeleton skeleton);

	// Receive only meshes from the Fbx root node
	FbxMesh* GetMeshFromRoot(FbxNode* node);

	void ProcessControlPoints(FbxNode* node);

	void ConvertToLeftHanded(FbxAMatrix &matrix);

private:

	Blend vertexBlend;
	unordered_map<unsigned int, ControlPoint*>controlPoints;
	FbxMesh* currentMesh;
	vector<int>indices;
	string animationName;
	FbxLongLong animationLength;

};



