#pragma once
#include "BufferComponents.h";
#include "GraphicComponents.h";
#include <DirectXCollision.h>;
using namespace DirectX;
//Project clone2
class Quadtree
{
public:
	Quadtree();
	Quadtree(int subDiv, XMFLOAT3 Center, XMFLOAT3 Extents);
	virtual ~Quadtree();

	//typedef struct vertex
	//{
	//	float x, y, z;
	//}VERTEX;
	//typedef struct node
	//{
	//	XMFLOAT3 BoundingCoords[4]; // four corners of the parent.
	//	float extentX; // thecurrent boxes width
	//	float extentZ; // the current boxes height.
	//	BoundingBox BBox;//Boundin box to test for intersection with meshes
	//	node* child[4];
	//	ID3D11Buffer* vtxBuffer;
	//}NODE;
	//NODE* Tree;
	Quadtree* nodes[4];
	//	XMFLOAT3 Bounding[4];
	BoundingBox BBox;
	int SubDiv;
	int totalSubDiv;
	//int vtxIndex[24] = { 0, 1, 2, 3, 0, 4, 7, 3, 0, 4, 5, 1, 1, 5, 6, 2, 2, 6, 7, 3, 4, 5, 6, 7 };
	int vtxIndex[16] = { 0, 1, 2, 3, 0, 4, 7, 3, 2, 6, 7, 4, 5, 1, 5, 6 };
	ID3D11Buffer* vtxBuffer;
	ID3D11Buffer* indexBuffer;


	bool CreateTree(int SubDiv, ID3D11Device* &gDevice);
	void release();
	void render(ID3D11DeviceContext* &gDeviceContext, GraphicComponents &gHandler);
	bool createIndex(ID3D11Device* &gDevice, ID3D11DeviceContext* &gDeviceContext);
private:



};
struct Vertices {
	XMFLOAT4 VtxArr;
};

//Vertices cubePoints[] = {
//	XMFLOAT4(float(Bounding[0].x), 0.0f, float(Bounding[0].z), 0.0f),//Top left
//	XMFLOAT4(float(Bounding[2].x), 0.0f, float(Bounding[2].z), 0.0f),//Bot left
//	XMFLOAT4(float(Bounding[3].x), 0.0f, float(Bounding[3].z), 0.0f),//Bot right
//	XMFLOAT4(float(Bounding[1].x), 0.0f, float(Bounding[1].z), 0.0f),//Top right
//	XMFLOAT4(float(Bounding[0].x), 10.0f, float(Bounding[0].z), 0.0f),//Top left offset
//	XMFLOAT4(float(Bounding[2].x), 10.0f, float(Bounding[2].z), 0.0f),//Bot left offset
//	XMFLOAT4(float(Bounding[3].x), 10.0f, float(Bounding[3].z), 0.0f),//Bot right offset
//	XMFLOAT4(float(Bounding[1].x), 10.0f, float(Bounding[1].z), 0.0f)//Top right offset
//};

////Create vertex buffer data for each child
//D3D11_BUFFER_DESC vtxDesc = {};
//vtxDesc.Usage = D3D11_USAGE_DEFAULT;
//vtxDesc.ByteWidth = sizeof(Vertices);
//vtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//
//D3D11_SUBRESOURCE_DATA data = {};
//data.pSysMem = cubePoints;