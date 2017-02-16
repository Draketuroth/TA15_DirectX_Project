#include "Quadtree.h";

Quadtree::Quadtree()
{

}
Quadtree::~Quadtree()
{

}
int Quadtree::calcNodeNr(int max, int min)
{
	int ctr = 0;
	int var = 0;

	while (var == 0)
	{
		ctr += max;
		max = max / min;
		if (max == 1)
		{
			var = 1;
		}
	}
	ctr++;
	return ctr;
}
