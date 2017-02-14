#include "Terrain.h"
#include <iostream>
//Test test

Terrain::Terrain()
{
	terrainInfo.HMapFilename = L"Textures\\HightMap.raw";
	terrainInfo.HMapHeight = 64;
	terrainInfo.HMapWidth = 64;

	//l�ngden mellan varje vertis
	terrainInfo.quadSize = 1;
	terrainInfo.HeightScale = 1; 

	NumPatchVertRows = (terrainInfo.HMapHeight - 1);
	NumPatchVertCols = (terrainInfo.HMapWidth - 1);

	NumPatchVertices = NumPatchVertRows*NumPatchVertCols; 
	NumPatchQuadFaces = (NumPatchVertRows - 1) * (NumPatchVertCols - 1);
	
}

Terrain::~Terrain()
{
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
		heightMap[i] = (in[i] / 20)*terrainInfo.HeightScale;
	}
}

bool Terrain::inBounds(int i, int j)
{
	bool valid = false; 
	if (i >= 0 && i < (int)terrainInfo.HMapHeight && j >= 0 && j < (int)terrainInfo.HMapWidth)
	{
		valid = true;
	}

	return valid; 
}

float Terrain::Average(int i, int j)
{
	//h�r g�r man igenom h�jden av elementen man j�mf�r och delar med den n�rmaste

	float avg = 0.0f; 
	float num = 0.0f; 

	for (int m = i - 1; m <= j + 1; ++m)
	{
		for (int n = j; n <= j + 1; ++n)
		{
			if (this->inBounds(m, n) == true)
			{
				avg += heightMap[m*terrainInfo.HMapWidth + n];
				num += 1.0f; 
			}
		}
	}

	return avg / num; 
	
}

//shaderResure view
void Terrain::BuildHeightmapSRV(ID3D11Device* device)
{
	HRESULT hr; 
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = terrainInfo.HMapWidth;
	texDesc.Height = terrainInfo.HMapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	vector<HALF> hmap(heightMap.size()); 
	transform<>(heightMap.begin(), heightMap.end(), hmap.begin(),XMConvertFloatToHalf);
	D3D11_SUBRESOURCE_DATA data; 
	data.pSysMem = &hmap[0]; 
	data.SysMemPitch = terrainInfo.HMapHeight * sizeof(HALF);
	data.SysMemSlicePitch = 0; 
	ID3D11Texture2D*hmapTex = 0; 
	hr = device->CreateTexture2D(&texDesc, &data, &hmapTex); 

	if (hr != S_OK)
	{
		cout << "Error CreateTexture 2D"; 
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; 
	srvDesc.Format = texDesc.Format; 
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; 
	srvDesc.Texture2D.MostDetailedMip = 0; 
	srvDesc.Texture2D.MipLevels = -1; 
	hr = (device->CreateShaderResourceView(hmapTex, &srvDesc, &heightmapSRV)); 

	if (hr != S_OK)
	{
		cout << "Error Shader Resourse View";
	}

	ReleaseCOM(hmapTex);
}

void Terrain::Smooth() 
{
	vector<float>dest(heightMap.size());

	for (UINT i = 0; i < terrainInfo.HMapHeight; ++i) {

		for (UINT j = 0; j < terrainInfo.HMapWidth; ++j) {

			dest[i * terrainInfo.HMapWidth + j] = Average(i, j);
		}
	}
	// Replace the old heightmap with the filtered one

	heightMap = dest;
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

	float halfWidth = 0.5f*GetWidth(); 
	float halfDepth = 0.5f*GetDepth(); 

	float patchWidth = GetWidth() / (NumPatchVertCols - 1); 
	float patchDepth = GetDepth() / (NumPatchVertRows - 1); 

	float du = 1.0f / (NumPatchVertCols - 1); 
	float dv = 1.0f / (NumPatchVertRows - 1); 

	for (UINT i = 0; i < NumPatchVertRows; i++)
	{
		float z = halfDepth - i*patchDepth; 
		for (UINT j = 0; j < NumPatchVertCols; ++j)
		{
			

			float x = -halfWidth + j*patchWidth; 
			float y = heightMap[(i*NumPatchVertCols + j)];
			patchVertices[i*NumPatchVertCols + j].Varr = XMFLOAT3(x, y, z);

			Average(x, z);
			//str�cka texturen �ver griden
			patchVertices[i*NumPatchVertCols + j].VTarr.x = j*du;
			patchVertices[i*NumPatchVertCols + j].VTarr.y = i*dv;

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
	//for (unsigned int y = 0; y < 640; y++)
	//{
	//	for (unsigned int x = 0; x < 640; x++)
	//	{
	//		//postion
	
	//		VertPos.Varr.x = x * 10; 
	//		VertPos.Varr.y = y * 10;
	//	

	//		VertPos.Varr.z = 0; 

	//		// Tex s�tt till 0, f�r den anv�nds inte
	//		VertPos.VTarr.x = 0; 
	//		VertPos.VTarr.y = 0;

	//		//informationen f�r normalen s�tt till 0
	//		VertPos.VNarr.x = 0; 
	//		VertPos.VNarr.y = 0;
	//		VertPos.VNarr.z = 0;

	//		terrainV.push_back(VertPos); 
	//	}
	//}
	int k = 0; 

	vector<int> VertPos(NumPatchQuadFaces * 4);

	for (UINT i = 0; i < NumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < NumPatchVertCols - 1; ++j)
		{
			VertPos[k] = i*NumPatchVertCols + j;
			VertPos[k + 1] = i*NumPatchVertCols + j + 1;

			VertPos[k + 2] = (i + 1)*NumPatchVertCols + j;
			VertPos[k + 3] = (i + 1)*NumPatchVertCols + j + 1;
			k += 4;
			indexCounter += 4;

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

XMFLOAT3 Terrain::GetPosition()const
{

	int x = 0; 
	int y = 0; 
	int z = 0; 

	XMFLOAT3 mapPos(x, y, z); 
	return mapPos; 
}