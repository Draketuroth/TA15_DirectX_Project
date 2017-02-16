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
		int bType;
		VERTEX BoundingCoords[4];
		int Branches[4];
		int ID;
		int ParentID;
	}NODE;
	NODE* QuadTree;


private:
	int calcNodeNr(int max, int min);
	bool CreateTree(int Bounding[4], int ParentID, int NodeID);


};
