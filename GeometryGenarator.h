#pragma once
#include <string>
//#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
using namespace DirectX; 
using namespace std; 

class GeometryGenarator
{
public:
	//för att skapa geomerti, kan användas till andra saker än griden
	struct Vertex
	{
		Vertex() {}; 
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv) 
			:Position(p), Normal(n), TexC(uv){}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float u, float v)
			:Position(px, py, pz), Normal(nx, ny, nz), TexC(u, v) {}

			XMFLOAT3 Position;
			XMFLOAT3 Normal; 
			XMFLOAT2 TexC; 
	};

	struct MechData
	{
		vector<Vertex> Vertices; 
		vector<unsigned int> indices; 
	};

	void Grid(float width, float depth, int m, int n, MechData& mechData);

private:

};

