#include "Quadtree.h";

Quadtree::Quadtree()
{
	this->Tree = new NODE;

	this->SubDiv = 0;
	this->totalSubDiv = 4;
	this->Tree->extentX = 32;
	this->Tree->extentZ = 32;
	for (int i = 0; i < 4; i++)
	{
		if (SubDiv != totalSubDiv)
		{
			this->Tree->child[i] = new NODE;
		}
		else
		{
			this->Tree->child[i] = nullptr;
		}
	}

	
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
void Quadtree::CreateTree(NODE* quadtree, XMFLOAT3 Bounding[4], int SubDiv)
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

		for (int i = 0; i < 4; i++)
		{
			quadtree->BoundingCoords[i] = Bounding[i];
		}
	}

	for (int i = 0; i < 4; i++)
	{


		//	quadtree->child[i]->BBox.Extents.x /= 2;
		//	quadtree->child[i]->BBox.Extents.z /= 2;
		quadtree->child[i]->extentX /= 2;
		quadtree->child[i]->extentZ /= 2;

		if (i == 0)
		{
			//Create indexbuffer for each child


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

			for (size_t j = 0; j < 4; j++)
				{
					quadtree->child[0]->BoundingCoords[j] = Bounding[j];
				}
			if (SubDiv != totalSubDiv)
				{		
					CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv+1);
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

			for (size_t j = 0; j < 4; j++)
			{
				quadtree->child[1]->BoundingCoords[j] = Bounding[j];
			}
			if (SubDiv != totalSubDiv)
			{
				CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv + 1);
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

			for (size_t j = 0; j < 4; j++)
			{
				quadtree->child[2]->BoundingCoords[j] = Bounding[j];
			}
			if (SubDiv != totalSubDiv)
			{
				CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv + 1);
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

			for (size_t j = 0; j < 4; j++)
			{
				quadtree->child[3]->BoundingCoords[j] = Bounding[j];
			}
			if (SubDiv != totalSubDiv)
			{
				CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv + 1);
			}
		}
	}
}
void Quadtree::initializeIndex(int &indexBuffer)
{
	int tempArray[24] = { 0, 1, 2, 3, 0, 4, 7, 3, 0, 4, 5, 1, 1, 5, 6, 2, 2, 6, 7, 3, 4, 5, 6, 7 };
	this->vtxIndicies = tempArray;
}

