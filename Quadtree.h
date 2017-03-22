
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

	Quadtree* nodes[4];//Varje node har 4 barn(�kar exponentiellt)
	BoundingBox BBox;//sj�lva boundingboxen f�r quadtreet
	vector<CubeObjects*> objects;//Lista med objekt(kuber)
	int SubDiv;//Nuvarande subdiv
	int totalSubDiv;//Totala subdivions p� tr�det
	int intersection;//Variabel f�r att kolla intersektion mellan frustum och quadtree (anv�nder globala macros, se MacroDefinitions.h)

	//Funktioner f�r quadtree (x) indikerar i vilken ordning de k�rs i inuti main filen
	/*(1)*/bool CreateTree(int SubDiv);//Skapar tr�det
	/*(2)*/void checkBoundingBox(CubeObjects &Object);//L�gger in object i leaf nodes(l�gsta subdiv)
	/*(3)*/void recursiveIntersect(Camera camera);//Kollar intersektion mellan frustum och quadtreet
	/*(4)*/void checkRenderObjects();//Kollar vilka objekt som ska renderas ut
private:

};
