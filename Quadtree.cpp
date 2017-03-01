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
	for (int i = 0; i < 4; i++)
	{
		if (this->Tree->child[i] != nullptr)
		{
			delete this->Tree->child[i];
		}
	}
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
		Bounding[2] = { -quadtree->extentX, 0, -quadtree->extentZ	};//Bot left
		Bounding[3] = { quadtree->extentX, 0, -quadtree->extentZ};//Bot Right

		quadtree->BBox.Center = XMFLOAT3{ 0, 0, 0};
		quadtree->BBox.Extents = XMFLOAT3{ quadtree->extentX , 0, quadtree->extentZ};

		for (int i = 0; i < 4; i++)
		{
			quadtree->BoundingCoords[i] = Bounding[i];
		}
	}
	
	//for (int i = 0; i < 4; i++)
	//{
	//	quadtree->child[i]->BBox.Extents.x /= 2;
	//	quadtree->child[i]->BBox.Extents.z /= 2;
	//	quadtree->child[i]->extentX /= 2;
	//	quadtree->child[i]->extentZ /= 2;

	//	if (i == 0)
	//	{
	//		quadtree->child[i]->BBox.Center.y = 0;
	//		quadtree->child[i]->BBox.Center.x = quadtree->BBox.Center.x - (quadtree->extentX / 2);
	//		quadtree->child[i]->BBox.Center.z = quadtree->BBox.Center.z + (quadtree->extentZ / 2);

	//		Bounding[0] = { 0 , 0, quadtree->extentZ };//Top left child 0
	//		Bounding[1] = { 0, 0, quadtree->extentZ };//Top Right child 0
	//		Bounding[2] = { 0, 0, quadtree->extentZ};//Bot left child 0
	//		Bounding[3] = { quadtree->extentX, 0, quadtree->extentZ};//Bot Right child 0
	//		for (size_t j = 0; j < 4; j++)
	//		{
	//			quadtree->child[0]->BoundingCoords[j] = Bounding[j];
	//		}

	//		if (SubDiv != totalSubDiv)
	//		{
	//			
	//			CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv+1);
	//		}

	//	}
	//	if (i == 1)
	//	{
	//		Bounding[0] = { quadtree->Width, 0, 0 };//Top left for child 1
	//		Bounding[1] = { quadtree->Width * 2, 0, 0 };//Top right for child 1
	//		Bounding[2] = { quadtree->Width, 0, quadtree->Height };//Bot left for child 1
	//		Bounding[3] = { quadtree->Width * 2, 0, quadtree->Height * 2 };//Bot right for child 1
	//		for (size_t k = 0; k < 4; k++)
	//		{
	//			quadtree->child[1]->BoundingCoords[k] = Bounding[k];
	//		}

	//		quadtree->BBox.Center = XMFLOAT3{ 64 - (quadtree->Width / 2), 0, quadtree->Height / 2 };
	//		quadtree->BBox.Extents = XMFLOAT3{ quadtree->Width / 2, 0, quadtree->Height / 2 };

	//		if (SubDiv != totalSubDiv)
	//		{
	//				
	//			CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv);
	//		}
	//	}
	//	if (i == 2)
	//	{
	//		Bounding[0] = { 0, 0, quadtree->Height };//Top left for child 2
	//		Bounding[1] = { quadtree->Width * 2, 0, quadtree->Height * 2 };//Top right for child 2
	//		Bounding[2] = { 0, 0, quadtree->Height };//Bot left for child 2
	//		Bounding[3] = { quadtree->Width * 2, 0, quadtree->Height * 2 };//Bot right for child 2
	//		for (size_t l = 0; l < 4; l++)
	//		{
	//			quadtree->child[2]->BoundingCoords[l] = Bounding[l];
	//		}

	//		quadtree->BBox.Center = XMFLOAT3{ quadtree->Width / 2, 0, -64 + (quadtree->Height / 2) };
	//		quadtree->BBox.Extents = XMFLOAT3{ quadtree->Width / 2, 0, quadtree->Height / 2 };

	//		if (SubDiv != totalSubDiv)
	//		{
	//			CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv+1);
	//		}
	//	}
	//	if (i == 3)
	//	{
	//		Bounding[0] = { quadtree->Width, 0, quadtree->Height };//Top left for child 3
	//		Bounding[1] = { quadtree->Width * 2, 0, quadtree->Height * 2 };//Top right for child 3
	//		Bounding[2] = { quadtree->Width, 0, quadtree->Height };//Bot left for child 3
	//		Bounding[3] = { quadtree->Width * 2, 0, quadtree->Height * 2 };//Bot right for child 3
	//		for (size_t m = 0; m < 4; m++)
	//		{
	//			quadtree->child[2]->BoundingCoords[m] = Bounding[m];
	//		}

	//		quadtree->BBox.Center = XMFLOAT3{ 64 - quadtree->Width / 2, 0, -64 + (quadtree->Height / 2) };
	//		quadtree->BBox.Extents = XMFLOAT3{ quadtree->Width / 2, 0, quadtree->Height / 2 };

	//		if (SubDiv != totalSubDiv)
	//		{
	//			CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv+1);
	//		}
	//	}
	//}
}
