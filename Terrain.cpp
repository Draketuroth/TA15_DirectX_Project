#include "Terrain.h"

Terrain::Terrain(){

}

Terrain::~Terrain(){


}

void Terrain::LoadHeightMap() {

	const wchar_t* fileName = L"Textures\\terrain.raw";
	terrainInfo.HMapWidth = 64;
	terrainInfo.HMapHeight = 64;

	// A height for each vertex
	vector<unsigned char>in(terrainInfo.HMapWidth * terrainInfo.HMapHeight);

	// Open the file
	
	ifstream inFile;
	inFile.open(fileName, ios_base::binary);

	if (inFile) {

		// Read the RAW bytes
		inFile.read((char*)&in[0], (streamsize)in.size());

		// When we are done with the file, close the ifstream

		inFile.close();
	}

	// Copy array data into a float array and scale it

	heightMap.resize(terrainInfo.HMapHeight * terrainInfo.HMapWidth, 0);

	for (UINT i = 0; i < terrainInfo.HMapHeight * terrainInfo.HMapWidth; ++i) {

		heightMap[i] = (in[i] / 255.0f) * terrainInfo.HeightScale;
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

void Terrain::BuildHeightmapSRV(ID3D11Device* device)
{
	InitInfo mInfo;
	 
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mInfo.HMapWidth;
	texDesc.Height = mInfo.HMapHeight;
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
	transform<>(heightMap.begin(), heightMap.end(), hmap.begin(), hmap.end(), multiplies <int>());
	D3D11_SUBRESOURCE_DATA data; 
	data.pSysMem = &hmap; 
	data.SysMemPitch = mInfo.HMapHeight * sizeof(HALF);
	data.SysMemSlicePitch = 0; 

	ID3D11Texture2D*hmapTex = 0; 
	HRESULT(device->CreateTexture2D(&texDesc, &data, &hmapTex)); 

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; 
	srvDesc.Format = texDesc.Format; 
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; 
	srvDesc.Texture2D.MostDetailedMip = 0; 
	srvDesc.Texture2D.MipLevels = -1; 
	HRESULT(device->CreateShaderResourceView(hmapTex, &srvDesc, &heightmapSRV)); 

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

void Terrain::CalculateGridDimension() {

	// Here we calculate the vertex grid dimensions

	static const int CellsPerPatch = 64;

	NumPatchVertRows = ((terrainInfo.HMapHeight - 1) / CellsPerPatch) + 1;
	NumPatchVertCols = ((terrainInfo.HMapWidth - 1) / CellsPerPatch) + 1;

	// The total number of patch vertices and quad patch primitives are calculated by:

	NumPatchVertices = NumPatchVertRows * NumPatchVertCols;
	NumPatchQuadFaces = (NumPatchVertRows - 1) * (NumPatchVertCols - 1);
}

float Terrain::GetWidth()const {

	// Return the total terrain width

	return(terrainInfo.HMapWidth - 1) * terrainInfo.CellSpacing;
}

float Terrain::GetDepth()const {

	// Return the total terrain depth

	return(terrainInfo.HMapHeight - 1) * terrainInfo.CellSpacing;
}

void Terrain::BuildQuadPatchVB(ID3D11Device* device) {

	HRESULT hr;

	vector<TerrainVertex>patchVertices(NumPatchVertRows * NumPatchVertCols);

	float halfWidth = 0.5 * GetWidth();
	float halfDepth = 0.5f * GetDepth();

	float patchWidth = GetWidth() / (NumPatchVertCols - 1);
	float patchDepth = GetDepth() / (NumPatchVertRows - 1);

	float du = 1.0f / (NumPatchVertCols - 1);
	float dv = 1.0f / (NumPatchVertRows - 1);

	for (UINT i = 0; i < NumPatchVertRows; i++) {

		float z = halfDepth - i * patchDepth;

		for (UINT j = 0; j < NumPatchVertCols; j++) {

			float x = -halfWidth + j * patchWidth;
			patchVertices[i * NumPatchVertCols + j].Pos = XMFLOAT3(x, 0.0f, z);

			// Stretch the texture over the grid

			patchVertices[i * NumPatchVertCols + j].Tex.x = j * du;
			patchVertices[i * NumPatchVertCols + j].Tex.y = i * dv;
		}
	}

	// Store axis-aligned bounding box y-bounds in the upper-left corner

	/*for (UINT i = 0; i < NumPatchVertRows - 1; i++) {

		for (UINT j = 0; j < NumPatchVertCols - 1; j++) {

			UINT patchID = i * (NumPatchVertCols - 1) + j;
			patchVertices[i * NumPatchVertCols + j].BoundsY = PatchBoundsY[patchID];
		}
	}*/

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(TerrainVertex) * patchVertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexInitData;
	vertexInitData.pSysMem = &patchVertices[0];
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexInitData, &mQuadPatchVB);

}

void Terrain::BuildQuadPatchIB(ID3D11Device* device) {

	HRESULT hr;

	// There are four indices per quad face

	vector<USHORT>indices(NumPatchQuadFaces * 4);

	// We must now iterate over each quad and compute the indices

	int k = 0;

	for (UINT i = 0; i < NumPatchVertRows - 1; i++) {

		for (UINT j = 0; j < NumPatchVertCols - 1; j++) {

			// This will be the top row of 2x2 quad patch

			indices[k] = i * NumPatchVertCols + j;
			indices[k + 1] = i * NumPatchVertCols + j + 1;

			// This will be the bottom row of 2x2 quad patch

			indices[k + 2] = (i + 1) * NumPatchVertCols + j;
			indices[k + 3] = (i + 1) * NumPatchVertCols + j + 1;
			k += 4; // Next quad
		}
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(USHORT) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = &indices[0];
	hr = (device->CreateBuffer(&indexBufferDesc, &indexInitData, &mQuadPatchIB));

}