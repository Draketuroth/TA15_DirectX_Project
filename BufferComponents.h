#ifndef BUFFERCOMPONENTS_H
#define BUFFERCOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include "FBXLoader.h"
#include "Terrain.h"
#include <iostream>
#include "MacroDefinitions.h"
#include "Camera.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "DirectXCollision.h"

using namespace DirectX;

// We require a combined transformation matrix from all the previously created matrices and a matrix to preserve the world positions throughout the pipeline

struct GS_CONSTANT_BUFFER {

	XMMATRIX lightViewProj;
	XMMATRIX worldViewProj;
	XMMATRIX matrixWorld;
	XMMATRIX matrixView;
	XMMATRIX matrixProjection;
	XMMATRIX viewProj;
	XMMATRIX floorRot;
	XMMATRIX matrixViewInverse;
	XMFLOAT4 cameraPos;
	XMFLOAT4 cameraUp;
	XMMATRIX worldInvTranspose;
	

};

struct VS_CONSTANT_BUFFER {

	XMFLOAT4 particleMovement[1000];

};

//Declspec helps to declare the bytewidth of the constant buffer
__declspec(align(16))
struct MTL_STRUCT
	{

	XMFLOAT4 Kd;
	XMFLOAT4 Ka;
	XMFLOAT4 Tf;
	XMFLOAT4 Ks;
	float Ni;
	float Illum;
	XMFLOAT2 padding;
	};

struct CylinderMeshData
{
	vector<Vertex_Cylinder> Vertices;
	vector<UINT> Indices;
};

struct CubeObjects {

	XMMATRIX objectWorldMatrix;
	ID3D11Buffer* gCubeVertexBuffer;;
	BoundingBox bbox;
	bool renderCheck;
};

struct TOPDOWN_CAMERA {

	XMMATRIX topDownViewTransform;
	XMMATRIX viewInverse;
};

class BufferComponents {
	
public:

	BufferComponents();
	~BufferComponents();
	void ReleaseAll();

	bool fileFound;
	vector<OBJStruct> ImportStruct;

	MTL_STRUCT MTLConstantData;
	VS_CONSTANT_BUFFER VtxConstantData;

	UINT cylinderVertexCount;
	UINT cylinderIndicesCount;

	XMMATRIX transformMatrix;
	XMMATRIX tWorldMatrix;
	XMMATRIX tFloorRot;
	wstring OBJTexturePath;
	
	XMMATRIX tLightViewProj;

	ID3D11Buffer* topDownCameraBuffer;
	TOPDOWN_CAMERA topDownCamData;

	vector<Vertex_Bone> fbxVector;
	
	CubeObjects cubeObjects[CUBECAPACITY];

	ID3D11Buffer* gVertexBuffer;	// Vertex buffer
	ID3D11Buffer* gTerrainBuffer;	// for OBJ parser
	ID3D11Buffer* gConstantBuffer;	// Constant buffer to provide the vertex shader with updated transformation data per frame
	ID3D11Buffer* gMTLBuffer;
	ID3D11Buffer* gVertexConstantBuffer;

	ID3D11Texture2D* depthStencil;	// Depth-stencil texture
	ID3D11DepthStencilState* depthState;	// Depth-stencil state used for the output merger
	ID3D11DepthStencilView* depthView;	// Depth-stencil view to access the depth stencil texture

	ID3D11RasterizerState* gRasteriserState;

	ID3D11Buffer* gCylinderBuffer;
	ID3D11Buffer* gCylinderIndexBuffer;

	ID3D11Buffer* gCubeIndexBuffer;

	ID3D11Buffer* gFrustumBuffer;
	ID3D11Buffer* gArrowBuffer;
	ID3D11Buffer* gFrustumIndexBuffer;

	bool SetupScene(ID3D11Device* &gDevice, Camera &mCam, FbxImport &fbxImporter);
	bool CreateTerrainBuffer(ID3D11Device* &gDevice);

	bool CreateVertexBuffer(ID3D11Device* &gDevice);
	bool CreateSkeletalBuffers(ID3D11Device* &gDevice, FbxImport &fbxImporter);
	bool CreateConstantBuffer(ID3D11Device* &gDevice, Camera &mCam);
	bool CreateOBJBuffer(ID3D11Device* &gDevice);
	bool CreateRasterizerState(ID3D11Device* &gDevice);

	bool CreateCylinderBuffers(ID3D11Device* &gDevice);
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, CylinderMeshData& meshData);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, CylinderMeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, CylinderMeshData& meshData);

	bool CreateVertexConstantBuffer(ID3D11Device* &gDevice);

	bool CreateCubeVertices(ID3D11Device* &gDevice);
	bool CreateCubeIndices(ID3D11Device* &gDevice);

	float RandomNumber(float Minimum, float Maximum);

	bool CreateTopDownCameraBuffer(ID3D11Device* &gDevice);
	
	bool CreateFrustumBuffer(ID3D11Device* &gDevice, XMFLOAT3 FrustumCorners[8]);
	bool CreateFrustumBuffer(ID3D11Device* &gDevice);
	bool CreateFrustumIndexBuffer(ID3D11Device* &gDevice);

};

#endif BUFFERCOMPONENTS_H
