#include "Quadtree.h"


Quadtree::Quadtree()
{
	this->SubDiv = 0;
	this->totalSubDiv = 1;
	this->BBox.Center = { 0, 0, 0 };
	this->BBox.Extents = { 32, 32, 32 };
	this->intersection = OUTSIDE;
	for (int i = 0; i < 4; i++)
	{
		this->nodes[i] = nullptr;
	}

}

Quadtree::Quadtree(int subDiv, XMFLOAT3 Center, XMFLOAT3 Extents)
{
	this->SubDiv = subDiv;
	this->totalSubDiv = 1;
	this->BBox.Center = Center;
	this->BBox.Extents = Extents;

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

bool Quadtree::CreateTree(int SubDiv)
{
	if (SubDiv == 0)
	{
		this->BBox.Center = { 0, 0, 0 };
		this->BBox.Extents = { 32, 32, 32 };

	}
	//Sätter nya extents för childs (halveras för varje subdiv)
	float halfExtentX = this->BBox.Extents.x / 2;
	float halfExtentZ = this->BBox.Extents.z / 2;

	XMFLOAT3 newExtent = {halfExtentX, 32, halfExtentZ};

	//Sätter ny centerpunkt för varje box (top left, top right, bot left, bot right)

	//Top left center
	XMFLOAT3 TLCenter = this->BBox.Center;
	TLCenter.x -= newExtent.x;
	TLCenter.z += newExtent.z;

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
		//Creating the children
		this->nodes[0] = new Quadtree(SubDiv + 1, TLCenter, newExtent);
		this->nodes[1] = new Quadtree(SubDiv + 1, TRCenter, newExtent);
		this->nodes[2] = new Quadtree(SubDiv + 1, BLCenter, newExtent);
		this->nodes[3] = new Quadtree(SubDiv + 1, BRCenter, newExtent);

		//Going into each children to create their children
		this->nodes[0]->CreateTree(SubDiv + 1);
		this->nodes[1]->CreateTree(SubDiv + 1);
		this->nodes[2]->CreateTree(SubDiv + 1);
		this->nodes[3]->CreateTree(SubDiv + 1);
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
					//if (this->nodes[i]->objects.size() > 0)
					//{

					//	//cout << this->nodes[i]->SubDiv << endl << this->nodes[i]->objects.size() << endl;
					//}
				}

			}
		}
	}
}

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
			if (this->nodes[i]->objects.size() > 0)//Check if the node has objects in the vector(if the node contains a cube)
			{
				//cout << "Size: " << this->nodes[i]->objects.size() << endl << "ID: " << this->nodes[i]->ID << endl;
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
