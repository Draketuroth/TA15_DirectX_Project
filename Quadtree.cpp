#include "Quadtree.h";
//Project clone2
Quadtree::Quadtree()
{
	this->SubDiv = 0;
	this->totalSubDiv = 4;
	this->BBox.Center = { 0, 0, 0 };
	this->BBox.Extents = { 0, 0, 0 };
	for (int i = 0; i < 4; i++)
	{
		this->nodes[i] = nullptr;
	}

}
Quadtree::Quadtree(int subDiv, XMFLOAT3 Center, XMFLOAT3 Extents)
{
	//for (size_t i = 0; i < 4; i++)
	//{
	//	this->Bounding[i] = Bounding[i];
	//} 
	this->SubDiv = subDiv;
	this->totalSubDiv = 4;
	this->BBox.Center = Center;
	this->BBox.Extents = Extents;
	for (UINT i = 0; i < 4; i++)
	{
		this->nodes[i] = nullptr;
	}

}
Quadtree::~Quadtree()
{
	for (UINT i = 0; i < 4; i++)
	{
		if (this->nodes[i] != nullptr)
		{
			delete this->nodes[i];
		}
	}
}
bool Quadtree::CreateTree(int SubDiv, ID3D11Device* &gDevice)
{
	if (SubDiv == 0)
	{
		this->BBox.Center = { 0, 0, 0 };
		this->BBox.Extents = { 32, 32, 32 };
	}
	XMVECTOR halfExtent = XMLoadFloat3(&this->BBox.Extents) / 2;
	XMFLOAT3 newExtent;
	XMStoreFloat3(&newExtent, halfExtent);

	//Top left center
	XMFLOAT3 TLCenter = this->BBox.Center;
	TLCenter.x -= newExtent.x;
	TLCenter.y += newExtent.y;

	//Top right center
	XMFLOAT3 TRCenter = this->BBox.Center;
	TRCenter.x += newExtent.x;
	TRCenter.z += newExtent.z;

	//Bot left center
	XMFLOAT3 BLCenter = this->BBox.Center;
	BLCenter.x -= newExtent.x;
	BLCenter.z -= newExtent.z;

	//Bot right center
	XMFLOAT3 BRCenter = this->BBox.Center;
	BRCenter.x += newExtent.x;
	BRCenter.z -= newExtent.z;


	XMFLOAT3 cubePoints[8];
	this->BBox.GetCorners(cubePoints);
	Vertices vertexArr[] =
	{
		XMFLOAT4(cubePoints[0].x, cubePoints[0].y, cubePoints[0].z, 1.0f),
		XMFLOAT4(cubePoints[1].x, cubePoints[1].y, cubePoints[1].z, 1.0f),
		XMFLOAT4(cubePoints[2].x, cubePoints[2].y, cubePoints[2].z, 1.0f),
		XMFLOAT4(cubePoints[3].x, cubePoints[3].y, cubePoints[3].z, 1.0f),
		XMFLOAT4(cubePoints[4].x, cubePoints[4].y, cubePoints[4].z, 1.0f),
		XMFLOAT4(cubePoints[5].x, cubePoints[5].y, cubePoints[5].z, 1.0f),
		XMFLOAT4(cubePoints[6].x, cubePoints[6].y, cubePoints[6].z, 1.0f),
		XMFLOAT4(cubePoints[7].x, cubePoints[7].y, cubePoints[7].z, 1.0f)
	};
	//Create vertex buffer data for each child
	D3D11_BUFFER_DESC vtxDesc = {};
	vtxDesc.Usage = D3D11_USAGE_DEFAULT;
	vtxDesc.ByteWidth = sizeof(Vertices);
	vtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertexArr;

	HRESULT hr = gDevice->CreateBuffer(&vtxDesc, &data, &this->vtxBuffer);
	if (FAILED(hr))
	{
		return false;
	}
	if (SubDiv != totalSubDiv)
	{
		this->nodes[0] = new Quadtree(SubDiv + 1, TLCenter, newExtent);
		this->nodes[1] = new Quadtree(SubDiv + 1, TRCenter, newExtent);
		this->nodes[2] = new Quadtree(SubDiv + 1, BLCenter, newExtent);
		this->nodes[3] = new Quadtree(SubDiv + 1, BRCenter, newExtent);
	}
	for (int i = 0; i < 4; i++)
	{
		if (SubDiv != totalSubDiv)
		{
			this->nodes[i]->CreateTree(SubDiv + 1, gDevice);
		}
	}
	return true;
}
void Quadtree::release()
{
	SAFE_RELEASE(this->vtxBuffer);

	for (int i = 0; i < 4; i++)
	{
		if (this->nodes[i] != nullptr)
		{
			this->nodes[i]->release();
		}
	}
}
void Quadtree::render(ID3D11DeviceContext* &gDeviceContext, GraphicComponents &gHandler)
{
	gDeviceContext->VSSetShader(gHandler.gVertexQTreeShader, nullptr, 0);
	gDeviceContext->PSSetShader(gHandler.gPixelQTreeShader, nullptr, 0);

	UINT vertexSize = sizeof(QuadtreeVertex);
	UINT offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &this->vtxBuffer, &vertexSize, &offset);
	
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	gDeviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gDeviceContext->IASetInputLayout(gHandler.gVertexQTreeLayout);

	gDeviceContext->Draw(24, 0);
	for (int i = 0; i < 4; i++)
	{
		if (this->nodes[i] != nullptr)
		{
			this->nodes[i]->render(gDeviceContext, gHandler);
		}
	}
}
bool Quadtree::createIndex(ID3D11Device* &gDevice, ID3D11DeviceContext* &gDeviceContext)
{
	D3D11_BUFFER_DESC indexDesc = {};
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.ByteWidth = sizeof(int) * 16;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = this->vtxIndex;

	HRESULT hr = gDevice->CreateBuffer(&indexDesc, &initData, &this->indexBuffer);
	if (FAILED(hr))
	{
		return false;
	}


	return true;
}
