#ifndef VERTEXTYPE_H
#define VERTEXTYPE_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <iostream>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

struct Vertex
{
	Vertex() {}
	Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
		: Position(p), Normal(n), TangentU(t), TexC(uv) {}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: Position(px, py, pz), Normal(nx, ny, nz),
		TangentU(tx, ty, tz), TexC(u, v) {}

	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT3 TangentU;
	XMFLOAT2 TexC;
};

struct PosNormalTexTan
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
	XMFLOAT3 TangentU;
};

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

	XMFLOAT3 posX;
};
struct QuadtreeVertex {
	float x, y, z;
};


#endif
