#pragma once
#include "BufferComponents.h";
#include <DirectXCollision.h>;
using namespace DirectX;
class Quadtree
{
public:
	Quadtree();
	~Quadtree();
	
	//typedef struct vertex
	//{
	//	float x, y, z;
	//}VERTEX;
	typedef struct node
	{
		XMFLOAT3 BoundingCoords[4]; // four corners of the parent.
		float extentX; // thecurrent boxes width
		float extentZ; // the current boxes height.
		BoundingBox BBox;//Boundin box to test for intersection with meshes
		node* child[4];
	}NODE;
	NODE* Tree;

	XMFLOAT3 Bounding[4];
	int SubDiv;
	int totalSubDiv;

	void CreateTree(NODE* quadtree, XMFLOAT3 Bounding[4], int SubDiv);
private:



};
