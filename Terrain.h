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
#include "MacroDefinitions.h"

using namespace std; 
using namespace DirectX;
using namespace DirectX::PackedVector;
class Terrain
{
public:
	struct InitInfo
	{
		//RAW file
		wstring HMapFilename; 

		//scale
		float HeightScale;

		//hight and Width
		UINT HMapWidth; 
		UINT HMapHeight;
		float quadSize;
	};

	Terrain();
	~Terrain();

	void LoadRAW();

	float GetWidth()const; 
	float GetDepth()const;
	//vector<int> GetHight()const;

	float GetHeight(float x, float z)const; 

	//vertex buffer
	void BuildQuadPatchVB(ID3D11Device* device); 
	//indexbuffer
	void BuildQuadPatchIB(ID3D11Device* device);

	ID3D11ShaderResourceView* heightmapSRV;
	ID3D11Buffer* mQuadPatchVB;
	ID3D11Buffer* mQuadPatchIB;

	//en objstruct = 1 vertecis
	unsigned int indexCounter = 0;

	vector<float> heightMap; 
	
	vector<int> VertPos;

private:

	
	vector<XMFLOAT2>PatchBoundsY;
	InitInfo terrainInfo;

	static const int cellperPatch = 64; 
	int NumPatchVertices;
	int NumPatchVertRows;
	int NumPatchVertCols;
	int NumPatchQuadFaces;

	
	
};
#endif
