
#include "BufferComponents.h"
#include "GraphicComponents.h"
#include <DirectXCollision.h>

using namespace DirectX;
//Project clone2
class Quadtree
{
public:
	Quadtree();
	Quadtree(int subDiv, XMFLOAT3 Center, XMFLOAT3 Extents);
	virtual ~Quadtree();

	Quadtree* nodes[4];//Varje node har 4 barn(ökar exponentiellt)
	BoundingBox BBox;//själva boundingboxen för quadtreet
	vector<CubeObjects*> objects;//Lista med objekt(kuber)
	int SubDiv;//Nuvarande subdiv
	int totalSubDiv;//Totala subdivions på trädet
	int intersection;//Variabel för att kolla intersektion mellan frustum och quadtree (använder globala macros, se MacroDefinitions.h)

	//Funktioner för quadtree (x) indikerar i vilken ordning de körs i inuti main filen
	/*(1)*/bool CreateTree(int SubDiv);//Skapar trädet
	/*(2)*/void checkBoundingBox(CubeObjects &Object);//Lägger in object i leaf nodes(lägsta subdiv)
	/*(3)*/void recursiveIntersect(Camera camera);//Kollar intersektion mellan frustum och quadtreet
	/*(4)*/void checkRenderObjects();//Kollar vilka objekt som ska renderas ut
private:

};
