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
		node* child[4];
		int subLvl;
	}NODE;
	NODE* Tree;
	float GridX;
	float GridZ;
	int SubDiv;
private:
	bool CreateTree(vertex Bounding[4], int SubDiv, int subLvl);
	void createRoot(vertex Bounding[4]);

};
