#pragma once
#include "BufferComponents.h";
#include "GraphicComponents.h";
#include <DirectXCollision.h>;
using namespace DirectX;

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
	

	void CreateTree(NODE* quadtree, XMFLOAT3 Bounding[4], int SubDiv, ID3D11Device* &gDevice);
	void DeleteFunction(NODE* Tree);
private:



};
struct Vertices {
	XMFLOAT4 VtxArr;
};