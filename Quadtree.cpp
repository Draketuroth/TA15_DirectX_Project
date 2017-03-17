#include "Quadtree.h"

//Project clone2
Quadtree::Quadtree()
{
	this->SubDiv = 0;
	this->totalSubDiv = 4;
	this->BBox.Center = { 0, 0, 0 };
	this->BBox.Extents = { 32, 32, 32 };
	this->BBox.Transform(this->BBox, this->WorldM);
	this->intersection = OUTSIDE;
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
	this->BBox.Transform(this->BBox, this->WorldM);
	this->intersection = OUTSIDE;
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
		this->BBox.Transform(this->BBox, this->WorldM);
		this->calculateHalfD();
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

		this->nodes[0]->calculateHalfD();
		this->nodes[1]->calculateHalfD();
		this->nodes[2]->calculateHalfD();
		this->nodes[3]->calculateHalfD();
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

void Quadtree::checkBoundingBox(CubeObjects &Object)
{

	if (this->BBox.Contains(Object.bbox))//Check if the current node has an object
	{
		if (this->SubDiv != totalSubDiv - 1)
		{
			for (int i = 0; i < 4; i++)//loops through children
			{

				// We want to check the lowest subdiv by checking not the actual totalsubDiv but the subdiv before the lowest meaning the parent to 
				// the lowest subdiv because we check nodes[i](children) therefore the lowest subdiv does not have children, so we just have to check 
				// the children of the parent to the lowest subdiv
		
				if (this->nodes[i]->BBox.Contains(Object.bbox)) //check if children on index I has an object
				{

				//if it isn't the deepest level of subdiv we will continue to travel down the subdivision to find the lowest subdiv the object belongs to
					
					this->nodes[i]->checkBoundingBox(Object);	

				}
			}
		}
		else
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (this->nodes[i]->BBox.Contains(Object.bbox))
				{
					this->nodes[i]->objects.push_back(&Object);//Put the object thats in the lowest subdiv in the list of objects for this node
					if (this->nodes[i]->objects.size() > 0)
					{

						cout << this->nodes[i]->SubDiv << endl << this->nodes[i]->objects.size() << endl;
					}
				}

			}
		}
	}
}

int Quadtree::frustumIntersect(Camera camera)
{
	float e = 0;
	float s = 0;
	int outCounter = 0;
	int inCounter = 0;
	for (size_t i = 0; i < 6; i++)
	{
		e = (this->halfDiag.x * abs(camera.Frustum[i].Normal.x)) + (this->halfDiag.y * abs(camera.Frustum[i].Normal.y)) + (this->halfDiag.z * abs(camera.Frustum[i].Normal.z));
		XMVECTOR c = XMLoadFloat3(&this->BBox.Center);//Center for BBox
		XMVECTOR n = XMLoadFloat3(&camera.Frustum[i].Normal); //normal for the frustum plane
		XMVECTOR cn = XMVector3Dot(c, n); // dot product between C and N
		XMFLOAT3 cnFloat;
		XMStoreFloat3(&cnFloat, cn);
		//Calculations down below is a formula that can be referenced to the RTR book
		s = cnFloat.x + camera.Frustum[i].Distance;
		if (s - e > 0)//is outside the plane
		{
			outCounter++;
		}
		if (s - e > 0 && outCounter == 5)
		{
			return OUTSIDE;
		}
		if (s + e < 0)//inside the plane
		{
			inCounter++;
		}
		if (s + e < 0 && inCounter == 5)
		{
			return INSIDE;
		}
	}

	return INTERSECT;
}

void Quadtree::calculateHalfD()
{
	
	XMFLOAT3 corners[8];
	float minX = 0;
	float minY = 0;
	float minZ = 0;
	float maxX = 0;
	float maxY = 0;
	float maxZ = 0;
	this->BBox.GetCorners(corners);
	for (size_t i = 0; i < 8; i++)
	{
		if (minX > corners[i].x)
		{
			minX = corners[i].x;
		}
		if (minY > corners[i].y)
		{
			minY = corners[i].y;
		}
		if (minZ > corners[i].z)
		{
			minZ = corners[i].z;
		}
		if (maxX < corners[i].x)
		{
			maxX = corners[i].x;
		}
		if (maxY < corners[i].y)
		{
			maxY = corners[i].y;
		}
		if (maxZ < corners[i].z)
		{
			maxZ = corners[i].z;
		}
	}

	XMFLOAT3 minCoord = { minX, minY, minZ };
	XMVECTOR vecMin = XMLoadFloat3(&minCoord);
	XMFLOAT3 maxCoord = { maxX, maxY, maxZ };
	XMVECTOR vecMax = XMLoadFloat3(&maxCoord);

	XMVECTOR h = (vecMax - vecMin) / 2;
	XMStoreFloat3(&this->halfDiag, h);

}

//void Quadtree::recursiveIntersect(Camera camera)
//{
//	if (SubDiv == 0)
//	{
//		if (this->frustumIntersect(camera) == INTERSECT || this->frustumIntersect(camera) == INSIDE)
//		{
//			this->intersection = INSIDE;
//		}
//		else
//		{
//			this->intersection = OUTSIDE;
//		}
//	}
//	for (size_t i = 0; i < 4; i++)
//	{
//		if (SubDiv != totalSubDiv)
//		{
//			if (this->nodes[i]->frustumIntersect(camera) == INSIDE || this->nodes[i]->frustumIntersect(camera) == INTERSECT)
//			{
//				this->nodes[i]->intersection = INSIDE;
//				this->nodes[i]->recursiveIntersect(camera);
//			}
//			else
//			{
//				this->nodes[i]->intersection = OUTSIDE;
//			}
//		}
//	}
//}

void Quadtree::recursiveIntersect(Camera camera)
{
	if (this->SubDiv == 0)
	{
		if (camera.testFrust.Intersects(this->BBox))
		{
			this->intersection = INTERSECT;
		}
		else
		{
			this->intersection = OUTSIDE;
		}
	}
	if (this->intersection != OUTSIDE)
	{
		if (this->SubDiv != totalSubDiv)
		{
			for (size_t i = 0; i < 4; i++)
			{
		
				if (camera.testFrust.Intersects(this->nodes[i]->BBox))
				{
					this->nodes[i]->intersection = INTERSECT;
					this->nodes[i]->recursiveIntersect(camera);
				}
				else
				{
					this->nodes[i]->intersection = OUTSIDE;
				}
			
			}
		}
	}
}
void Quadtree::checkRenderObjects()
{
	for (size_t i = 0; i < 4; i++)//Loops through children
	{
		if (this->SubDiv != totalSubDiv - 1)//do not stop until the second lowest subdiv
		{
		//if the node is inside the frustum we will continue to check for objects inside the node
			this->nodes[i]->checkRenderObjects();		
		}
		else//here we will stop at the second lowest subdiv, because we check the second lowest subdiv Quadtrees children, which is the lowes subdiv
		{
			if (this->nodes[i]->objects.size() > 0)
			{
				cout << this->nodes[i]->objects.size() << endl;
				for (size_t j = 0; j < this->nodes[i]->objects.size(); j++)
				{
					if (this->nodes[i]->intersection != OUTSIDE)
					{
						this->nodes[i]->objects[j]->renderCheck = true;
					}
					else
					{
						this->nodes[i]->objects[j]->renderCheck = false;
					}
				}	
			}			
		}
	}

}