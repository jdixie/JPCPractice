#ifndef _GRIDMAP_H
#define _GRIDMAP_H
#define WIN32_LEAN_AND_MEAN

#include <vector>
#include "constants.h"

//define the hexmap and pathing for said map
struct GridSpace
{
	float x1, x2, y1, y2, z1, z2; //lower left, right, lower vert, higher, etc
	int id;
};

class Gridmap
{
private:
	std::vector<float> gridPoint;
	std::vector<GridSpace> gridSpace;

public:
	void initialize();
	GridSpace getGridSpace(float x1c)
	{

	}

	GridSpace getGridSpace(int id)
	{

	}
};

#endif