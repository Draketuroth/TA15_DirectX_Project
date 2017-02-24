#include "Quadtree.h";

Quadtree::Quadtree()
{
	this->Tree = new NODE;

	this->SubDiv = 0;
	this->totalSubDiv = 4;
	this->Tree->Height = -64;
	this->Tree->Width = 64;
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

void Quadtree::CreateTree(NODE* quadtree, vertex Bounding[4], int SubDiv)
{
	
	//Skapa "big box", och fyra childs, varje child kommer vara en ny "big box" i nästa funktion
	if (SubDiv == 0)
	{
		//quadtree->Height = 64;
		//quadtree->Width = 64;
		Bounding[0] = { 0, 0, 0 };//Top left
		Bounding[1] = { quadtree->Width, 0, 0 };//Top Right
		Bounding[2] = { 0, 0, quadtree->Height};//Bot left
		Bounding[3] = { quadtree->Width, 0, quadtree->Height};//Bot Right

		for (int i = 0; i < 4; i++)
		{
			quadtree->BoundingCoords[i] = Bounding[i];
		}
	}
	
	for (size_t i = 0; i < 4; i++)
	{
		if (i == 0)
		{
				 
			Bounding[0] = { 0, 0, 0 };//Top left child 0
			Bounding[1] = { quadtree->Width/2, 0, 0 };//Top Right child 0
			Bounding[2] = { 0, 0, quadtree->Height/2 };//Bot left child 0
			Bounding[3] = { quadtree->Width/2, 0, quadtree->Height/2 };//Bot Right child 0
			for (size_t j = 0; j < 4; j++)
			{
				quadtree->child[0]->BoundingCoords[j] = Bounding[j];
			}

			quadtree->child[i]->Width = quadtree->child[i]->Width / 2;
			quadtree->child[i]->Height= quadtree->child[i]->Height / 2;
			if (SubDiv != totalSubDiv)
			{
				
				CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv+1);
			}

		}
		if (i == 1)
		{
			Bounding[0] = { quadtree->Width / 2, 0, 0 };//Top left for child 1
			Bounding[1] = { quadtree->Width, 0, 0 };//Top right for child 1
			Bounding[2] = { quadtree->Width / 2, 0, quadtree->Height / 2 };//Bot left for child 1
			Bounding[3] = { quadtree->Width, 0, quadtree->Height / 2 };//Bot right for child 1
			for (size_t k = 0; k < 4; k++)
			{
				quadtree->child[1]->BoundingCoords[k] = Bounding[k];
			}
			quadtree->child[i]->Width = quadtree->child[i]->Width / 2;
			quadtree->child[i]->Height = quadtree->child[i]->Height / 2;
			if (SubDiv != totalSubDiv)
			{
					
				CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv);
			}
		}
		if (i == 2)
		{
			Bounding[0] = { 0, 0, quadtree->Height / 2 };//Top left for child 2
			Bounding[1] = { quadtree->Width / 2, 0, quadtree->Height / 2 };//Top right for child 2
			Bounding[2] = { 0, 0, quadtree->Height };//Bot left for child 2
			Bounding[3] = { quadtree->Width / 2, 0, quadtree->Height };//Bot right for child 2
			for (size_t l = 0; l < 4; l++)
			{
				quadtree->child[2]->BoundingCoords[l] = Bounding[l];
			}
			quadtree->child[i]->Width = quadtree->child[i]->Width / 2;
			quadtree->child[i]->Height = quadtree->child[i]->Height / 2;
			if (SubDiv != totalSubDiv)
			{
				CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv+1);
			}
		}
		if (i == 3)
		{
			Bounding[0] = { quadtree->Width / 2, 0, quadtree->Height / 2 };//Top left for child 3
			Bounding[1] = { quadtree->Width, 0, quadtree->Height / 2 };//Top right for child 3
			Bounding[2] = { quadtree->Width / 2, 0, quadtree->Height };//Bot left for child 3
			Bounding[3] = { quadtree->Width, 0, quadtree->Height };//Bot right for child 3
			for (size_t m = 0; m < 4; m++)
			{
				quadtree->child[2]->BoundingCoords[m] = Bounding[m];
			}
			quadtree->child[i]->Width = quadtree->child[i]->Width / 2;
			quadtree->child[i]->Height = quadtree->child[i]->Height / 2;
			if (SubDiv != totalSubDiv)
			{
				CreateTree(quadtree->child[i], quadtree->child[i]->BoundingCoords, SubDiv+1);
			}
		}
	}
}
