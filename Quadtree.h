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
		VERTEX BoundingCoords[4]; // four corners of the parent.
		float Width; // thecurrent boxes width
		float Height; // the current boxes height.
		node* child[4];
	}NODE;
	NODE* Tree;

	int SubDiv;
	int totalSubDiv;
private:
	void CreateTree(NODE* quadtree, vertex Bounding[4], int &SubDiv);


};
