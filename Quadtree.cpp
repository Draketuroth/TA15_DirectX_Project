#include "Quadtree.h";

Quadtree::Quadtree()
{
	this->Tree = nullptr;
	this->SubDiv = 3;
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

	if (SubDiv == 0)
	{
		Bounding[0] = { 0, 0, 0 }; //Top-Left our origin point
		Bounding[1] = { GridX, 0, 0 }; //Top-Right
		Bounding[2] = { 0, 0, GridZ }; //Bot-Left
		Bounding[3] = { GridX, 0, GridZ }; //Bot-Right
	}
	this->Tree->child[0]->BoundingCoords[0] = Bounding[0];
	this->Tree->child[1]->BoundingCoords[1] = Bounding[1];

	if (subLvl != SubDiv)
	{
		CreateTree(Bounding, subLvl++, subLvl++);
	}

	return created;
}
void Quadtree::createRoot(vertex Bounding[4])
{
	bool created = false;

	Bounding[0] = { 0, 0, 0 }; //Top-Left our origin point
	Bounding[1] = { GridX, 0, 0 }; //Top-Right
	Bounding[2] = { 0, 0, GridZ }; //Bot-Left
	Bounding[3] = { GridX, 0, GridZ }; //Bot-Right
}