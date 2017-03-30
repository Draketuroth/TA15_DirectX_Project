#include "Terrain.h"
#include <iostream>
//Test test

Terrain::Terrain()
{
	terrainInfo.HMapFilename = L"Textures\\HMap.raw";
	terrainInfo.HMapHeight = 64;
	terrainInfo.HMapWidth = 64; 

	//l�ngden mellan varje vertis
	terrainInfo.quadSize = 1;
	//maxh�jd
	terrainInfo.HeightScale = 15; 

	NumPatchVertRows = terrainInfo.HMapHeight;
	NumPatchVertCols = terrainInfo.HMapWidth;

	NumPatchVertices = NumPatchVertRows*NumPatchVertCols; 
	NumPatchQuadFaces = (NumPatchVertRows - 1) * (NumPatchVertCols - 1);
}

Terrain::~Terrain()
{
	

}

void Terrain::ReleaseAll() {

	SAFE_RELEASE(heightmapSRV);
	SAFE_RELEASE(mQuadPatchVB);
	SAFE_RELEASE(mQuadPatchIB);
	
}

//load RAW file to heightMap
void Terrain::LoadRAW()
{
	//tar emot h�jden f�r var vertex
	vector<unsigned char> in(this->terrainInfo.HMapWidth * terrainInfo.HMapHeight);
	
	//open file
	ifstream inFile;
	inFile.open(terrainInfo.HMapFilename.c_str(), std::ios_base::binary);
	if (!inFile)
	{
		cout << "File not found" << endl; 
	}

	if (inFile)
	{
		//read RAW bytes
		inFile.read((char*)&in[0], (std::streamsize)in.size()); 

		//done
		inFile.close(); 
	}
	//copy the array data into a float array and scale it
	heightMap.resize(terrainInfo.HMapHeight * terrainInfo.HMapWidth, 0);
	for (UINT i = 0; i < terrainInfo.HMapHeight * terrainInfo.HMapWidth; i++)
	{
		//h�r sparar vi h�jdv�rdena, och delar med 255
		heightMap[i] = (in[i]/255.0f)*terrainInfo.HeightScale;
	}
}


float Terrain::GetWidth()const
{
	return (terrainInfo.HMapWidth - 1)*terrainInfo.quadSize;
}

float Terrain::GetDepth()const
{
	return (terrainInfo.HMapHeight - 1)*terrainInfo.quadSize;
}

void Terrain::BuildQuadPatchVB(ID3D11Device* device)
{
	//antal vertiser
	vector<OBJStruct> patchVertices(NumPatchVertRows*NumPatchVertCols); 

	//anv�nder halv l�ngderna f�r att f� origo i mitten av terrengen
	float halfWidth = 0.5f*GetWidth(); 
	float halfDepth = 0.5f*GetDepth(); 
	//var ruta/cells storlek
	float patchWidth = GetWidth() / (NumPatchVertCols - 1); 
	float patchDepth = GetDepth() / (NumPatchVertRows - 1); 
	//uv till texturen
	float du = 1.0f / (NumPatchVertCols - 1); 
	float dv = 1.0f / (NumPatchVertRows - 1); 

	//ger varje vertex positionsv�rden
	int k = 0; 
	for (UINT i = 0; i < NumPatchVertRows; ++i)
	{
		float z = 0;
		for (UINT j = 0; j < NumPatchVertCols; ++j)
		{
			float x = 0; 
			float y = 0;
			
			x = -halfDepth +j ;
			//h�r ger man vertisen sin h�jd
			y = heightMap[i*NumPatchVertCols + j];
			z = halfDepth - i;

			patchVertices[i*NumPatchVertCols + j].Varr = XMFLOAT3(x, y, z);

			//str�cka texturen �ver griden
			patchVertices[i*NumPatchVertCols + j].VTarr.y = j*du;
			patchVertices[i*NumPatchVertCols + j].VTarr.x = i*dv;
		 	
			k++;
		}
	}
	
	D3D11_BUFFER_DESC vbd; 
	vbd.Usage = D3D11_USAGE_DEFAULT; 
	vbd.ByteWidth = sizeof(OBJStruct) * patchVertices.size(); 
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; 
	vbd.CPUAccessFlags = 0; 
	vbd.MiscFlags = 0; 
	vbd.StructureByteStride = 0; 

	D3D11_SUBRESOURCE_DATA vinitData; 
	vinitData.pSysMem = &patchVertices[0]; 
	HRESULT(device->CreateBuffer(&vbd, &vinitData, &mQuadPatchVB)); 

}

void Terrain::BuildQuadPatchIB(ID3D11Device* device)
{
	HRESULT hr;
	int k = 0; 

	VertPos.resize(NumPatchVertices * 6); 
	//h�r bet�ttar vi vilken ordning vi skriver ut vertiserna
	//ger en Triangel list
	for (unsigned int i = 0; i < NumPatchVertRows -1; i++)
	{
		for (unsigned int j = 0; j < NumPatchVertCols -1; j++)
		{
			VertPos[k + 5] = (i + 1) * NumPatchVertCols + j + 1;
			VertPos[k + 4] = i * NumPatchVertCols + j + 1;
			VertPos[k + 3] = (i + 1) * NumPatchVertCols + j;
			
			VertPos[k + 2] = (i + 1) * NumPatchVertCols + j;
			VertPos[k + 1] = i * NumPatchVertCols + j + 1;
			VertPos[k] = i * NumPatchVertCols + j;

			//next quad
			k += 6; 
			indexCounter += 6; 

		}
	}

	D3D11_BUFFER_DESC ibd;
	memset(&ibd, 0, sizeof(ibd));
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0; 
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.ByteWidth = VertPos.size() * sizeof(int);

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = VertPos.data();
	hr = device->CreateBuffer(&ibd, &iinitData, &mQuadPatchIB);

	if (hr != S_OK)
	{
		cout << "Error Index buffer" << endl; 
	}
}

float Terrain::GetHeight(float x, float z)const
{
	//f�rsta basbyttet med en skalning och translation, g�r cellerna relativa till varandra
	//c och d �r postionen fr�n camran i nya basen
	float c = (x + 0.5f*GetWidth()) / terrainInfo.quadSize;
	float d = (z - 0.5f*GetDepth()) / -terrainInfo.quadSize;

	int row = (int)floorf(d); 
	int col = (int)floorf(c); 

	// ABCD �r h�rnen p� en quad
	float A = heightMap[(row*terrainInfo.HMapWidth) + col]; 
	float B = heightMap[row*terrainInfo.HMapWidth + col + 1]; 
	float C = heightMap[(row + 1)*terrainInfo.HMapWidth + col]; 
	float D = heightMap[(row + 1)* terrainInfo.HMapWidth + col + 1]; 

	//n�sta basbyte, nu �r s och t camrans postion. Och vi �r inne p� den specefica cellen vi st�r p�
	float s = c - (float)col; 
	float t = d - (float)row; 
	//kollar vilken del av quaden vi �r i och retunerar interpolerad h�jd
	if (s + t <= 1.0f)
	{
		float uy = B - A; 
		float vy = C - A; 
		return A + s*uy + t*vy; 
	}
	else
	{
		float uy = C - D; 
		float vy = B - D; 
		return D + (1.0f - s)*uy + (1.0f - t)*vy; 
	}
	//sedan anv�nder vi denna funkionen i main 
}