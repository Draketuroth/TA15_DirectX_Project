#include "Quadtree.h";

Quadtree::Quadtree()
{
	this->Tree = nullptr;
	this->Tree->SubDivs = 3;
	this->Tree->subLvl = 0;
	this->GridZ = 64;
	this->GridX = 64;
}
Quadtree::~Quadtree()
{

}

bool Quadtree::CreateTree(vertex Bounding[4], int SubDiv, int subLvl)
{
	bool created = false;
	Bounding[0] = { 0, 0, 0 }; //Top-Left our origin point
	Bounding[1] = { GridX, 0, 0 }; //Top-Right
	Bounding[2] = { GridZ, 0, };


	if (subLvl != SubDiv)
	{
		CreateTree(Bounding, subLvl++, subLvl++);
	}

	return created;
}