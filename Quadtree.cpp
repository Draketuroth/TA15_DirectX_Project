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
void Quadtree::checkIntersect(BoundingBox &Object)
{

	for (int i = 0; i < 4; i++)
	{

	}

}