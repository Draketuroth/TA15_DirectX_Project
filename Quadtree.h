#ifndef QUADTREE_H
#define QUADTREE_H
//#include "BufferComponents.h";
#include "GraphicComponents.h";
#include <DirectXCollision.h>;
//using namespace DirectX;
//unsigned int vtxIndex[24] = { 0, 1, 2, 3, 0, 4, 7, 3, 0, 4, 5, 1, 1, 5, 6, 2, 2, 6, 7, 3, 4, 5, 6, 7 };
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
		int SubDiv;
		int totalSubDiv;
		XMFLOAT3 BoundingCoords[4]; // four corners of the parent.
		float extentX; // thecurrent boxes width
		float extentZ; // the current boxes height.
		BoundingBox BBox;//Boundin box to test for intersection with meshes
		node* child[4] = { nullptr, nullptr, nullptr, nullptr };
		ID3D11Buffer* vtxBuffer;
		node() {
			SubDiv = 0;
			totalSubDiv = 4;
			if (SubDiv != totalSubDiv)
			{
				for (int i = 0; i < 4; i++)
				{
					child[i] = new node;
				}
			}
			extentX = 32;
			extentZ = 32;
		}
	}NODE;
	NODE* Tree;
	ID3D11Buffer* indexBuffer = nullptr;

	XMFLOAT3 Bounding[4];
	
	
	int vtxIndex[24] = { 0, 1, 2, 3, 0, 4, 7, 3, 0, 4, 5, 1, 1, 5, 6, 2, 2, 6, 7, 3, 4, 5, 6, 7 };
	
	void renderTree(NODE* quadtree, ID3D11DeviceContext* &gDeviceContext);
	void createIndexBuffer(ID3D11Device* &gDevice, ID3D11DeviceContext* &gDeviceContext);
	void CreateTree(NODE* quadtree, XMFLOAT3 Bounding[4], int SubDiv, ID3D11Device* &gDevice);
	void DeleteFunction(NODE* Tree);
private:



};
struct Vertices {
	XMFLOAT4 VtxArr;
};
#endif