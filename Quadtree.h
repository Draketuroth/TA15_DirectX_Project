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

	Quadtree* nodes[4];
	BoundingBox BBox;
	vector<BoundingBox> objects;
	int SubDiv;
	int totalSubDiv;
	//int vtxIndex[24] = { 0, 1, 2, 3, 0, 4, 7, 3, 0, 4, 5, 1, 1, 5, 6, 2, 2, 6, 7, 3, 4, 5, 6, 7 };
//	int vtxIndex[16] = { 0, 1, 2, 3, 0, 4, 7, 3, 2, 6, 7, 4, 5, 1, 5, 6 };



	bool CreateTree(int SubDiv, ID3D11Device* &gDevice);
	void release();
	void render(ID3D11DeviceContext* &gDeviceContext, GraphicComponents &gHandler);
	bool createIndex(ID3D11Device* &gDevice, ID3D11DeviceContext* &gDeviceContext);
	void checkIntersect(BoundingBox &Object);
private:

};
struct Vertices {
	XMFLOAT4 VtxArr;
};
