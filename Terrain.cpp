#include "Terrain.h"
#include <iostream>
Terrain::Terrain()
{
	terrainInfo.HMapFilename = L"Textures\\terrain.raw";
	terrainInfo.HMapHeight = 64;
	terrainInfo.HMapWidth = 64;
	//längden mellan varje vertis
	terrainInfo.quadSize = 1;

	NumPatchVertRows = ((terrainInfo.HMapHeight - 1) / cellperPatch) + 1;
	NumPatchVertCols = ((terrainInfo.HMapWidth - 1) / cellperPatch) + 1;

	NumPatchVertices = NumPatchVertRows*NumPatchVertCols; 
	NumPatchQuadFaces = (NumPatchVertRows - 1) * (NumPatchVertCols - 1);
	
}

Terrain::~Terrain()
{
}

//load RAW file to heightMap
void Terrain::LoadRAW()
{
	

	//tar emot höjden för var vertex
	vector<unsigned char> in(this->terrainInfo.HMapWidth * terrainInfo.HMapHeight);
	
	//open file
	ifstream inFile;
	inFile.open(terrainInfo.HMapFilename.c_str(), std::ios_base::binary);
	if (inFile)
	{
		//read RAW bytes
		inFile.read((char*)&in[0], (std::streamsize)in.size()); 

		//done
		inFile.close(); 
	}
	else
	{
		cout << "Error RAWfile" << endl; 
	}

	//copy the array data into a float array and scale it
	heightMap.resize(terrainInfo.HMapHeight * terrainInfo.HMapWidth, 0);
	for (UINT i = 0; i < terrainInfo.HMapHeight * terrainInfo.HMapWidth; i++)
	{
		heightMap[i] = (in[i] / 255.0f)*terrainInfo.HeightScale;
	}
}

bool Terrain::inBounds(int i, int j)
{
	InitInfo mInfo; 
	bool valid = false; 
	if (i >= 0 && i < (int)mInfo.HMapWidth && j >= 0 && j < (int)mInfo.HMapWidth)
	{
		valid = true;
	}

	return valid; 
}

float Terrain::Average(int i, int j)
{
	//här går man igenom höjden av elementen man jämför och delar med den närmaste

	float avg = 0.0f; 
	float num = 0.0f; 
	InitInfo mInfo; 

	for (int m = i - 1; m <= j + 1; ++m)
	{
		for (int n = j; n <= j + 1; ++n)
		{
			if (this->inBounds(m, n))
			{
				avg += heightMap[m*mInfo.HMapWidth + n]; 
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

void Terrain::Smooth() {

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
	vector<Vertex::terrain> patchVertices(NumPatchVertRows*NumPatchVertCols); 

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
			patchVertices[i*NumPatchVertCols + j].Position = XMFLOAT3(x, 0.0f, z); 

			//sträcka texturen över griden
			patchVertices[i*NumPatchVertCols + j].Texture.x = j*du; 
			patchVertices[i*NumPatchVertCols + j].Texture.y = i*dv; 
		}
	}

	D3D11_BUFFER_DESC vbd; 
	vbd.Usage = D3D11_USAGE_DEFAULT; 
	vbd.ByteWidth = sizeof(Vertex::terrain) * patchVertices.size(); 
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

	//4 indices per quad face
	vector<USHORT> indices(NumPatchQuadFaces * 4); 
	//iterate over eachquad and compute indices
	USHORT a; 
	USHORT b; 
	USHORT c; 
	USHORT d;

	int k = 0; 
	for (UINT i = 0; i < NumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < NumPatchVertCols - 1; ++j)
		{
			//indices.push_back(k	* NumPatchVertCols + j); 

			//top row of 2x2 quad patch
			a = indices[k] = i * NumPatchVertCols + j; 
			b = indices[k + 1] = i * NumPatchVertCols + j + 1; 

			//bottom row 2x2 quad patch
			c = indices[k + 2] = (i + 1)*NumPatchVertCols + j; 
			d = indices[k + 3] = (i + 1)*NumPatchVertCols + j + 1;
			//next quad
			k += 4; 

			indices.push_back(a); 
			indices.push_back(b);
			indices.push_back(c);
			indices.push_back(d);
		}
	}

	D3D11_BUFFER_DESC ibd;
	memset(&ibd, 0, sizeof(ibd));
	ibd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0; 
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.ByteWidth = terrainV.size() * sizeof(OBJStruct);

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices.data();
	hr = device->CreateBuffer(&ibd, &iinitData, &mQuadPatchIB);

	if (hr != S_OK)
	{
		cout << "Error Index buffer" << endl; 
	}
}