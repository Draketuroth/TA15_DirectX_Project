#include "Quadtree.h";

Quadtree::Quadtree()
{
	this->Tree = new NODE;




	
//	this->CreateTree(this->Tree, this->Bounding, this->SubDiv);
}
Quadtree::~Quadtree()
{
	DeleteFunction(this->Tree);
}
void Quadtree::DeleteFunction(NODE* Tree)
{
	for (size_t i = 0; i < 4; i++)
	{

		if (Tree->child[i] != nullptr)
		{
			DeleteFunction(Tree->child[i]);
		}
		else
		{
			delete Tree;
		}
	}
	delete Tree;
}
void Quadtree::CreateTree(NODE* quadtree, XMFLOAT3 Bounding[4], int SubDiv, ID3D11Device* &gDevice)
{
	//Skapa "big box", och fyra childs, varje child kommer vara en ny "big box" i nästa funktion
	if (SubDiv == 0)
	{
		//quadtree->Height = 64;
		//quadtree->Width = 64;
		Bounding[0] = { -quadtree->extentX , 0, quadtree->extentZ };//Top left
		Bounding[1] = { quadtree->extentX, 0, quadtree->extentZ };//Top Right
		Bounding[2] = { -quadtree->extentX, 0, -quadtree->extentZ };//Bot left
		Bounding[3] = { quadtree->extentX, 0, -quadtree->extentZ };//Bot Right

		quadtree->BBox.Center = XMFLOAT3{ 0, 0, 0 };
		quadtree->BBox.Extents = XMFLOAT3{ quadtree->extentX , 0, quadtree->extentZ };


		Vertices cubePoints[] = {
			XMFLOAT4(float(Bounding[0].x), 0.0f, float(Bounding[0].z), 0.0f),//Top left
			XMFLOAT4(float(Bounding[2].x), 0.0f, float(Bounding[2].z), 0.0f),//Bot left
			XMFLOAT4(float(Bounding[3].x), 0.0f, float(Bounding[3].z), 0.0f),//Bot right
			XMFLOAT4(float(Bounding[1].x), 0.0f, float(Bounding[1].z), 0.0f),//Top right
			XMFLOAT4(float(Bounding[0].x), 10.0f, float(Bounding[0].z), 0.0f),//Top left offset
			XMFLOAT4(float(Bounding[2].x), 10.0f, float(Bounding[2].z), 0.0f),//Bot left offset
			XMFLOAT4(float(Bounding[3].x), 10.0f, float(Bounding[3].z), 0.0f),//Bot right offset
			XMFLOAT4(float(Bounding[1].x), 10.0f, float(Bounding[1].z), 0.0f)//Top right offset
		};
		//Create vertex buffer data for each child
		D3D11_BUFFER_DESC vtxDesc = {};
		vtxDesc.Usage = D3D11_USAGE_DEFAULT;
		vtxDesc.ByteWidth = sizeof(Vertices);
		vtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = cubePoints;

		HRESULT hr = gDevice->CreateBuffer(&vtxDesc, &data, &quadtree->vtxBuffer);


		for (int i = 0; i < 4; i++)
		{
			quadtree->BoundingCoords[i] = Bounding[i];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		quadtree->child[i] = new NODE;

		//	quadtree->child[i]->BBox.Extents.x /= 2;
		//	quadtree->child[i]->BBox.Extents.z /= 2;
		if (quadtree->child[i] != nullptr)
		{
			quadtree->child[i]->extentX = quadtree->extentX / 2;
			quadtree->child[i]->extentZ = quadtree->extentZ / 2;
		}


		if (i == 0)
		{
			quadtree->child[i]->BBox.Center.y = 0;
			quadtree->child[i]->BBox.Center.x = quadtree->BBox.Center.x - (quadtree->extentX / 2);
			quadtree->child[i]->BBox.Center.z = quadtree->BBox.Center.z + (quadtree->extentZ / 2);
			XMVECTOR tempExtent = XMLoadFloat3(&quadtree->BBox.Extents) / 2;
			XMStoreFloat3(&quadtree->child[i]->BBox.Extents ,tempExtent);
			
			Bounding[0] = quadtree->child[i]->BBox.Center;//Top left child 0
			Bounding[0].x -= quadtree->child[i]->BBox.Extents.x;
			Bounding[0].z += quadtree->child[i]->BBox.Extents.z;

			Bounding[1] = quadtree->child[i]->BBox.Center;//Top right child 0
			Bounding[1].x += quadtree->child[i]->BBox.Extents.x;
			Bounding[1].z += quadtree->child[i]->BBox.Extents.z;
	
			Bounding[2] = quadtree->child[i]->BBox.Center;//Bot left child 0
			Bounding[2].x -= quadtree->child[i]->BBox.Extents.x;
			Bounding[2].z -= quadtree->child[i]->BBox.Extents.z;

			Bounding[3] = quadtree->child[i]->BBox.Center;//Bot right child 0
			Bounding[3].x += quadtree->child[i]->BBox.Extents.x;
			Bounding[3].z -= quadtree->child[i]->BBox.Extents.z;

			Vertices cubePoints[] = {
				XMFLOAT4(float(Bounding[0].x), 0.0f, float(Bounding[0].z), 0.0f),//Top left
				XMFLOAT4(float(Bounding[2].x), 0.0f, float(Bounding[2].z), 0.0f),//Bot left
				XMFLOAT4(float(Bounding[3].x), 0.0f, float(Bounding[3].z), 0.0f),//Bot right
				XMFLOAT4(float(Bounding[1].x), 0.0f, float(Bounding[1].z), 0.0f),//Top right
				XMFLOAT4(float(Bounding[0].x), 10.0f, float(Bounding[0].z), 0.0f),//Top left offset
				XMFLOAT4(float(Bounding[2].x), 10.0f, float(Bounding[2].z), 0.0f),//Bot left offset
				XMFLOAT4(float(Bounding[3].x), 10.0f, float(Bounding[3].z), 0.0f),//Bot right offset
				XMFLOAT4(float(Bounding[1].x), 10.0f, float(Bounding[1].z), 0.0f)//Top right offset
			};
			//Create vertex buffer data for each child
			D3D11_BUFFER_DESC vtxDesc = {};
			vtxDesc.Usage = D3D11_USAGE_DEFAULT;
			vtxDesc.ByteWidth = sizeof(Vertices);
			vtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = cubePoints;

			HRESULT hr = gDevice->CreateBuffer(&vtxDesc, &data, &quadtree->child[i]->vtxBuffer);


			for (size_t j = 0; j < 4; j++)
				{
					quadtree->child[0]->BoundingCoords[j] = Bounding[j];
				}
			if (SubDiv != quadtree->child[i]->totalSubDiv)
				{		
					CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv + 1, gDevice);
				}
		}
		if (i == 1)
		{
			quadtree->child[i]->BBox.Center.y = 0;
			quadtree->child[i]->BBox.Center.x = quadtree->BBox.Center.x + (quadtree->extentX / 2);
			quadtree->child[i]->BBox.Center.z = quadtree->BBox.Center.z + (quadtree->extentZ / 2);
			XMVECTOR tempExtent = XMLoadFloat3(&quadtree->BBox.Extents) / 2;
			XMStoreFloat3(&quadtree->child[i]->BBox.Extents, tempExtent);

			Bounding[0] = quadtree->child[i]->BBox.Center;//Top left child 1
			Bounding[0].x -= quadtree->child[i]->BBox.Extents.x;
			Bounding[0].z += quadtree->child[i]->BBox.Extents.z;

			Bounding[1] = quadtree->child[i]->BBox.Center;//Top right child 1
			Bounding[1].x += quadtree->child[i]->BBox.Extents.x;
			Bounding[1].z += quadtree->child[i]->BBox.Extents.z;

			Bounding[2] = quadtree->child[i]->BBox.Center;//Bot left child 1
			Bounding[2].x -= quadtree->child[i]->BBox.Extents.x;
			Bounding[2].z -= quadtree->child[i]->BBox.Extents.z;

			Bounding[3] = quadtree->child[i]->BBox.Center;//Bot right child 1
			Bounding[3].x += quadtree->child[i]->BBox.Extents.x;
			Bounding[3].z -= quadtree->child[i]->BBox.Extents.z;

			Vertices cubePoints[] = {
				XMFLOAT4(float(Bounding[0].x), 0.0f, float(Bounding[0].z), 0.0f),//Top left
				XMFLOAT4(float(Bounding[2].x), 0.0f, float(Bounding[2].z), 0.0f),//Bot left
				XMFLOAT4(float(Bounding[3].x), 0.0f, float(Bounding[3].z), 0.0f),//Bot right
				XMFLOAT4(float(Bounding[1].x), 0.0f, float(Bounding[1].z), 0.0f),//Top right
				XMFLOAT4(float(Bounding[0].x), 10.0f, float(Bounding[0].z), 0.0f),//Top left offset
				XMFLOAT4(float(Bounding[2].x), 10.0f, float(Bounding[2].z), 0.0f),//Bot left offset
				XMFLOAT4(float(Bounding[3].x), 10.0f, float(Bounding[3].z), 0.0f),//Bot right offset
				XMFLOAT4(float(Bounding[1].x), 10.0f, float(Bounding[1].z), 0.0f)//Top right offset
			};
			//Create vertex buffer data for each child
			D3D11_BUFFER_DESC vtxDesc = {};
			vtxDesc.Usage = D3D11_USAGE_DEFAULT;
			vtxDesc.ByteWidth = sizeof(Vertices);
			vtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = cubePoints;

			HRESULT hr = gDevice->CreateBuffer(&vtxDesc, &data, &quadtree->child[i]->vtxBuffer);


			for (size_t j = 0; j < 4; j++)
			{
				quadtree->child[1]->BoundingCoords[j] = Bounding[j];
			}
			if (SubDiv != quadtree->child[i]->totalSubDiv)
			{
				CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv + 1, gDevice);
			}
		}
		if (i == 2)
		{
			quadtree->child[i]->BBox.Center.y = 0;
			quadtree->child[i]->BBox.Center.x = quadtree->BBox.Center.x - (quadtree->extentX / 2);
			quadtree->child[i]->BBox.Center.z = quadtree->BBox.Center.z - (quadtree->extentZ / 2);
			XMVECTOR tempExtent = XMLoadFloat3(&quadtree->BBox.Extents) / 2;
			XMStoreFloat3(&quadtree->child[i]->BBox.Extents, tempExtent);

			Bounding[0] = quadtree->child[i]->BBox.Center;//Top left child 2
			Bounding[0].x -= quadtree->child[i]->BBox.Extents.x;
			Bounding[0].z += quadtree->child[i]->BBox.Extents.z;

			Bounding[1] = quadtree->child[i]->BBox.Center;//Top right child 2
			Bounding[1].x += quadtree->child[i]->BBox.Extents.x;
			Bounding[1].z += quadtree->child[i]->BBox.Extents.z;

			Bounding[2] = quadtree->child[i]->BBox.Center;//Bot left child 2
			Bounding[2].x -= quadtree->child[i]->BBox.Extents.x;
			Bounding[2].z -= quadtree->child[i]->BBox.Extents.z;

			Bounding[3] = quadtree->child[i]->BBox.Center;//Bot right child 2
			Bounding[3].x += quadtree->child[i]->BBox.Extents.x;
			Bounding[3].z -= quadtree->child[i]->BBox.Extents.z;

			Vertices cubePoints[] = {
				XMFLOAT4(float(Bounding[0].x), 0.0f, float(Bounding[0].z), 0.0f),//Top left
				XMFLOAT4(float(Bounding[2].x), 0.0f, float(Bounding[2].z), 0.0f),//Bot left
				XMFLOAT4(float(Bounding[3].x), 0.0f, float(Bounding[3].z), 0.0f),//Bot right
				XMFLOAT4(float(Bounding[1].x), 0.0f, float(Bounding[1].z), 0.0f),//Top right
				XMFLOAT4(float(Bounding[0].x), 10.0f, float(Bounding[0].z), 0.0f),//Top left offset
				XMFLOAT4(float(Bounding[2].x), 10.0f, float(Bounding[2].z), 0.0f),//Bot left offset
				XMFLOAT4(float(Bounding[3].x), 10.0f, float(Bounding[3].z), 0.0f),//Bot right offset
				XMFLOAT4(float(Bounding[1].x), 10.0f, float(Bounding[1].z), 0.0f)//Top right offset
			};
	//Create vertex buffer data for each child
			D3D11_BUFFER_DESC vtxDesc = {};
			vtxDesc.Usage = D3D11_USAGE_DEFAULT;
			vtxDesc.ByteWidth = sizeof(Vertices);
			vtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = cubePoints;

			HRESULT hr = gDevice->CreateBuffer(&vtxDesc, &data, &quadtree->child[i]->vtxBuffer);

			for (size_t j = 0; j < 4; j++)
			{
				quadtree->child[2]->BoundingCoords[j] = Bounding[j];
			}
			if (SubDiv != quadtree->child[i]->totalSubDiv)
			{
				CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv + 1, gDevice);
			}
		}
		if (i == 3)
		{
			quadtree->child[i]->BBox.Center.y = 0;
			quadtree->child[i]->BBox.Center.x = quadtree->BBox.Center.x + (quadtree->extentX / 2);
			quadtree->child[i]->BBox.Center.z = quadtree->BBox.Center.z - (quadtree->extentZ / 2);
			XMVECTOR tempExtent = XMLoadFloat3(&quadtree->BBox.Extents) / 2;
			XMStoreFloat3(&quadtree->child[i]->BBox.Extents, tempExtent);

			Bounding[0] = quadtree->child[i]->BBox.Center;//Top left child 3
			Bounding[0].x -= quadtree->child[i]->BBox.Extents.x;
			Bounding[0].z += quadtree->child[i]->BBox.Extents.z;

			Bounding[1] = quadtree->child[i]->BBox.Center;//Top right child 3
			Bounding[1].x += quadtree->child[i]->BBox.Extents.x;
			Bounding[1].z += quadtree->child[i]->BBox.Extents.z;

			Bounding[2] = quadtree->child[i]->BBox.Center;//Bot left child 3
			Bounding[2].x -= quadtree->child[i]->BBox.Extents.x;
			Bounding[2].z -= quadtree->child[i]->BBox.Extents.z;

			Bounding[3] = quadtree->child[i]->BBox.Center;//Bot right child 3
			Bounding[3].x += quadtree->child[i]->BBox.Extents.x;
			Bounding[3].z -= quadtree->child[i]->BBox.Extents.z;

			Vertices cubePoints[] = {
				XMFLOAT4(float(Bounding[0].x), 0.0f, float(Bounding[0].z), 0.0f),//Top left
				XMFLOAT4(float(Bounding[2].x), 0.0f, float(Bounding[2].z), 0.0f),//Bot left
				XMFLOAT4(float(Bounding[3].x), 0.0f, float(Bounding[3].z), 0.0f),//Bot right
				XMFLOAT4(float(Bounding[1].x), 0.0f, float(Bounding[1].z), 0.0f),//Top right
				XMFLOAT4(float(Bounding[0].x), 10.0f, float(Bounding[0].z), 0.0f),//Top left offset
				XMFLOAT4(float(Bounding[2].x), 10.0f, float(Bounding[2].z), 0.0f),//Bot left offset
				XMFLOAT4(float(Bounding[3].x), 10.0f, float(Bounding[3].z), 0.0f),//Bot right offset
				XMFLOAT4(float(Bounding[1].x), 10.0f, float(Bounding[1].z), 0.0f)//Top right offset
			};
			//Create vertex buffer data for each child
			D3D11_BUFFER_DESC vtxDesc = {};
			vtxDesc.Usage = D3D11_USAGE_DEFAULT;
			vtxDesc.ByteWidth = sizeof(Vertices);
			vtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = cubePoints;

			HRESULT hr = gDevice->CreateBuffer(&vtxDesc, &data, &quadtree->child[i]->vtxBuffer);

			for (size_t j = 0; j < 4; j++)
			{
				quadtree->child[3]->BoundingCoords[j] = Bounding[j];
			}
			if (SubDiv != quadtree->child[i]->totalSubDiv)
			{
				CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv + 1, gDevice);
			}
		}
	}
}
void Quadtree::createIndexBuffer(ID3D11Device* &gDevice, ID3D11DeviceContext* &gDeviceContext)
{
	D3D11_BUFFER_DESC indexDesc = {};
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.ByteWidth = sizeof(unsigned int) * 24;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;


	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vtxIndex;
	HRESULT hr = gDevice->CreateBuffer(&indexDesc, &initData, &indexBuffer);
	if (FAILED(hr))
	{
		cout << "Couldnt create index buffer for quadtree" << endl;
	}
	

	//gDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

}
void Quadtree::renderTree(NODE* quadtree, ID3D11DeviceContext* &gDeviceContext)
{
	UINT vertexSize = sizeof(QuadtreeVertex);
	UINT offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &Tree->vtxBuffer, &vertexSize, &offset);

	gDeviceContext->Draw(24, 0);
	for (int i = 0; i < 4; i++)
	{
		if (Tree->child[i] != nullptr)
		{
			renderTree(Tree->child[i], gDeviceContext);
		}
	}
}