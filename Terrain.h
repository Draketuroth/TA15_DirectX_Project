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
		wstring HMapFilename0; 

		//Texture filnemes för hightmapen
		wstring HMapFilename1;
		wstring HMapFilename2;
		wstring HMapFilename3; 
		wstring HMapFilename4; 
		wstring HMapFilename; 

		//skala
		float HeightScale;

		//hight and Width
		UINT HMapWidth; 
		UINT HMapHeight;

		//the cell spacing along the x- and z- axes 
		float CellSpacing;
	};

	Terrain();
	~Terrain();

	void LoadHeightMap();
	bool inBounds(int i, int j); 
	float Average(int i, int j);

	//ifall terreringen behöver smooth
	void Smooth(); 

	void CalculateGridDimension();
	float GetWidth()const;
	float GetDepth()const;
	void BuildHeightmapSRV(ID3D11Device* device);
	void BuildQuadPatchVB(ID3D11Device* device);
	void BuildQuadPatchIB(ID3D11Device* device);

	ID3D11ShaderResourceView* heightmapSRV;
	ID3D11Buffer* mQuadPatchVB;
	ID3D11Buffer* mQuadPatchIB;
	
private:

	vector<float> heightMap; 
	vector<XMFLOAT2>PatchBoundsY;
	InitInfo terrainInfo;

	int NumPatchVertRows;
	int NumPatchVertCols;

	int NumPatchVertices;
	int NumPatchQuadFaces;

};


#endif
