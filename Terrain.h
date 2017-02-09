#ifndef TERRAIN_H
#define TERRAIN_H
#define ReleaseCOM(x)
#include <d3d11.h>
#include <d3dcompiler.h>	
#include <iostream>
//För att använda HALF
#include <DirectXPackedVector.h>
#include <vector>
#include <DirectXMath.h>
#include <Windows.h>
#include "VertexType.h"
#include <fstream>
#include <algorithm>

using namespace std; 
using namespace DirectX;
using namespace DirectX::PackedVector;
class Terrain
{
public:
	struct InitInfo
	{
		//här måste vi först hämta hightmapen
		wstring HMapFilename; 

		//skala
		float HeightScale;

		//hight and Width
		UINT HMapWidth; 
		UINT HMapHeight;

		//the cell spacing along the x- and z- axes 
		//med andra ord hur långt det är mellan varje vertis
		float quadSize;
	};

	Terrain();
	~Terrain();

	bool inBounds(int i, int j); 
	float Average(int i, int j);
	void LoadRAW();
	void Smooth(); 
	void BuildHeightmapSRV(ID3D11Device* device);
	float GetWidth()const; 
	float GetDepth()const; 
	void BuildQuadPatchVB(ID3D11Device* device); 
	void BuildQuadPatchIB(ID3D11Device* device);

	ID3D11ShaderResourceView* heightmapSRV;
	ID3D11Buffer* mQuadPatchVB;
	ID3D11Buffer* mQuadPatchIB;

	//en objstruct = 1 vertecis
	vector<OBJStruct> terrainV; 
	
private:

	vector<float> heightMap; 
	vector<XMFLOAT2>PatchBoundsY;
	InitInfo terrainInfo;

	static const int cellperPatch = 64; 
	int NumPatchVertRows;
	int NumPatchVertCols;

	int NumPatchVertices;
	int NumPatchQuadFaces;
	
};

//en egen namespace som håller två structer
namespace Vertex
{
	struct Basic32
	{
		XMFLOAT3 Position; 
		XMFLOAT3 Normal; 
		XMFLOAT2 Texture; 
	};

	struct terrain
	{
		XMFLOAT3 Position;
		XMFLOAT2 Texture;
		XMFLOAT2 BoundsY;
	};
}
#endif
