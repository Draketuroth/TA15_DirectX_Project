#pragma once
#include "BufferComponents.h";

class Quadtree
{
public:
	Quadtree();
	~Quadtree();

	typedef struct vertex
	{
		float x, y, z;
	}VERTEX;
	typedef struct node
	{
		VERTEX BoundingCoords[4];
		VERTEX center;
		int SubDivs;
		int subLvl;
	}NODE;
	NODE* Tree;
	float GridX;
	float GridZ;

private:
	bool CreateTree(vertex Bounding[4], int SubDiv, int subLvl);


};
