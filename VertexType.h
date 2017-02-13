#ifndef VERTEXTYPE_H
#define VERTEXTYPE_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <iostream>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

struct Vertex_Bone { // Stores the attributes of a vertex such as position, uv coordinates and normals

	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	float weights[4];
	UINT boneIndices[4];

};

struct Vertex_Standard {

	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;

};

struct OBJStruct
{
	XMFLOAT3 Varr;
	XMFLOAT2 VTarr;
	XMFLOAT3 VNarr;
	
	
};

struct TriangleVertex{

	float x, y, z;
	float u, v;
};



#endif
