#include "Quadtree.h";

Quadtree::Quadtree()
{
	this->Tree = nullptr;
	this->Tree->SubDivs = 3;
	this->Tree->subLvl = 1;
	this->GridZ = 64;
	this->GridX = 64;
}
Quadtree::~Quadtree()
{

}

bool Quadtree::CreateTree(vertex Bounding[4], int SubDiv, int subLvl)
{
	bool created = false;
	
	Bounding[0] = { GridX / -2, 0, GridZ / 2 };//Top left
	Bounding[1] = { GridX / 2, 0, GridZ / 2 };//Top right
	Bounding[2] = { GridX / -2, 0, GridZ / -2 };//Bot left
	Bounding[3] = { GridX / 2, 0, GridZ / -2 };//Bot right
	if (subLvl != SubDiv)
	{
		CreateTree(Bounding, subLvl++, subLvl++);
	}

	return created;
}