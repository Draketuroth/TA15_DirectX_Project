#pragma once
#include "BufferComponents.h";
#include <DirectXCollision.h>;
using namespace DirectX;
int vtxIndicies[24] = { 0, 1, 2, 3, 0, 4, 7, 3, 0, 4, 5, 1, 1, 5, 6, 2, 2, 6, 7, 3, 4, 5, 6, 7 };
class Quadtree
{
public:
	Quadtree();
	virtual ~Quadtree();
	
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
		ID3D11Buffer* vtxBuffer;
	}NODE;
	NODE* Tree;

	XMFLOAT3 Bounding[4];
	int SubDiv;
	int totalSubDiv;
	static const int vtxIndicies[24];

	void initializeIndex(int &indexBuffer);
	void CreateTree(NODE* quadtree, XMFLOAT3 Bounding[4], int SubDiv);
	void DeleteFunction(NODE* Tree);
private:



};
