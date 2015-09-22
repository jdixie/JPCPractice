#ifndef _OBJECT_H
#define _OBJECT_H
#define WIN32_LEAN_AND_MEAN

#include "Image.h"
#include "constants.h"
#include "Collisions.h"

extern GridContents collisionGrid[GRID_HASH_SIZE];

namespace ObjectNS
{
	enum COLLISION_TYPE { NONE, SPHERE, AABB, OBB};
//    const float GRAVITY = 6.67428e-11f;         //gravitational constant
	const float GRAVITY = 100;
}

struct Point
{
	float x;
	float y;
	float z;
};

struct AABoundingBox
{
	Point min;
	Point max;
};

struct OBoundingBox
{
	Point a;
};

struct BoundingSphere
{
	float radius;
};

class Object : public Image
{
protected:
	bool active;
	long gridLoc[8];		//collision grid index location
	GridCell gridCell;

	ObjectNS::COLLISION_TYPE collisionType;
	AABoundingBox aabb;
	OBoundingBox obb;
	BoundingSphere bs;
	DirectX::XMFLOAT3 corner[8];
	DirectX::XMFLOAT3 edgeX, edgeY, edgeZ;
//	float edgeXMin, edgeXMax, edgeYMin, edgeYMax, edgeZMin, edgeZMax;

	//physics related
	DirectX::XMFLOAT3 velocity;       //velocity
	DirectX::XMFLOAT3 deltaV;         //added to velocity during next call to update()
	float   mass;           //Mass of Object
	float   health;         //health, unsure if we will use percentile or values
	float	powerMeter;		//special attack/whatever pool
	float   force;          //Force of gravity
	float   gravity;        //gravitational constant of the game universe
	float	jumpForce;

public:
	Object();
	virtual ~Object();

	//set collision grid cell
	virtual void setGridCells();

	//set collision grid array index
	virtual void setGridLoc();

	virtual GridCell *getGridCell() { return &gridCell; }
	virtual long *getGridLoc() { return gridLoc; }
	virtual long getGridLoc(int i) { return gridLoc[i]; }

	virtual void setInitialGridCells();

	virtual void update(float frameTime);
	virtual bool collidesWith(Object *obj, DirectX::XMFLOAT3 *collisionVector);
	virtual bool collideAABoundingBox(Object *obj, DirectX::XMFLOAT3 *collisionVector);
	virtual bool collideOBoundingBox(Object *obj, DirectX::XMFLOAT3 *collisionVector);
	virtual void computeRotatedBox();
	virtual void createPrimaryBoundingBox();

	virtual void setVelocity(DirectX::XMFLOAT3 newVelocity){ velocity = newVelocity; }
	virtual DirectX::XMFLOAT3 getVelocity(){ return velocity; }
};

#endif