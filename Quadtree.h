
#include "BufferComponents.h"
#include "GraphicComponents.h"
#include <DirectXCollision.h>

using namespace DirectX;
//Project clone2
class Quadtree
{
public:
	Quadtree();
	Quadtree(int subDiv, XMFLOAT3 Center, XMFLOAT3 Extents, int ID);
	virtual ~Quadtree();

	Quadtree* nodes[4];
	BoundingBox BBox;
	vector<CubeObjects*> objects;
	int SubDiv;
	int totalSubDiv;
	int intersection;


	//int vtxIndex[24] = { 0, 1, 2, 3, 0, 4, 7, 3, 0, 4, 5, 1, 1, 5, 6, 2, 2, 6, 7, 3, 4, 5, 6, 7 };
	//int vtxIndex[16] = { 0, 1, 2, 3, 0, 4, 7, 3, 2, 6, 7, 4, 5, 1, 5, 6 };
	//XMFLOAT3 halfDiag;
	int ID;
	//XMMATRIX WorldM;


	bool CreateTree(int SubDiv);
	void checkBoundingBox(CubeObjects &Object);
	void recursiveIntersect(Camera camera);
	void checkRenderObjects();


	//void printIntersections();
	//void render(ID3D11DeviceContext* &gDeviceContext, GraphicComponents &gHandler);
	//int frustumIntersect(Camera camera);
	//bool createBuffer(ID3D11Device* &gDevice);
	//void calculateHalfD();

private:

};
//struct Vertices {
//	XMFLOAT4 VtxArr;
//};
