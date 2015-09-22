#include "object.h"

//extern GridContents collisionGrid[GRID_HASH_SIZE];

Object::Object()
{
	collisionType = ObjectNS::AABB;
	gridCell.x = -1;
	gridCell.y = -1;
	gridCell.z = -1;
	gridCell.next = NULL;
	gridLoc[0] = -1;
}

Object::~Object()
{
	//delete gridCell.next;
}

void Object::update(float frameTime)
{
//	tempX = getX() + velocity.x; //* gameScale;
//	tempY = getY() + velocity.y; //* gameScale;
//	tempZ = getZ() + velocity.z; //* gameScale;

	imageInfo.x = imageInfo.x + velocity.x; //* gameScale;
	imageInfo.y = imageInfo.y + velocity.y; //* gameScale;
	imageInfo.z = imageInfo.z + velocity.z; //* gameScale;

	moveVector.x = velocity.x;
	moveVector.y = velocity.y;
	moveVector.z = velocity.z;

	//for now, only update grid for this object if there was a change in velocity
	if (velocity.x != 0 || velocity.y != 0 || velocity.z != 0)
		setGridCells();
//	setGridLoc();
}

void Object::setGridCells()
{
	return;
	GridCell cell[8];
	bool xSplit = false;
	bool ySplit = false;
	bool zSplit = false;

	//remove old entries from grid collision table
	//moved operation to game.cpp updateCollisionGrid, maybe - nope nvm, do it here
	for (int index = 0; index <= 7; index++)
	{
		if (gridLoc[index] == -1)
		{
			break;
		}
		GridCell *tempCell = &gridCell;
		while (collisionGrid[gridLoc[index]].gridCell.x != tempCell->x ||
			collisionGrid[gridLoc[index]].gridCell.y != tempCell->y ||
			collisionGrid[gridLoc[index]].gridCell.z != tempCell->z)
		{
			if (collisionGrid[gridLoc[index]].next != NULL)
				tempCell = tempCell->next;
			else
				throw(GameError(gameErrorNS::FATAL_ERROR, L"Error with grid collision system - error removing object during GridCell search."));

			if (tempCell == NULL)
				throw(GameError(gameErrorNS::FATAL_ERROR, L"Error with grid collision system - error removing object GridCell not matched."));
		}
		//remove from object list
		ObjectPtrList *tempObject = collisionGrid[gridLoc[index]].object;
		if (tempObject != NULL)
		{
			if (tempObject->next != NULL)
			{
				while (tempObject->next->objectPtr != this)
				{
					tempObject = tempObject->next;
					if (tempObject == NULL)
						throw(GameError(gameErrorNS::FATAL_ERROR, L"Error with grid collision system - error removing object, not in list."));
				}
				tempObject->next = tempObject->next->next;
			}
		}
	}

	//remove all gridCell entries
	if (gridCell.next != NULL)
	{
		if (gridCell.next != NULL)
			delete gridCell.next;
	}

	//compute cells of the objects 8 corners
	cell[0].x = (imageInfo.x - imageInfo.rx) / GRID_UNIT;
	cell[0].y = (imageInfo.y + imageInfo.ry) / GRID_UNIT;
	cell[0].z = (imageInfo.z - imageInfo.rz) / GRID_UNIT;

	cell[1].x = (imageInfo.x - imageInfo.rx) / GRID_UNIT;
	cell[1].y = (imageInfo.y + imageInfo.ry) / GRID_UNIT;
	cell[1].z = (imageInfo.z + imageInfo.rz) / GRID_UNIT;

	cell[2].x = (imageInfo.x + imageInfo.rx) / GRID_UNIT;
	cell[2].y = (imageInfo.y + imageInfo.ry) / GRID_UNIT;
	cell[2].z = (imageInfo.z + imageInfo.rz) / GRID_UNIT;

	cell[3].x = (imageInfo.x + imageInfo.rx) / GRID_UNIT;
	cell[3].y = (imageInfo.y + imageInfo.ry) / GRID_UNIT;
	cell[3].z = (imageInfo.z - imageInfo.rz) / GRID_UNIT;

	cell[4].x = (imageInfo.x - imageInfo.rx) / GRID_UNIT;
	cell[4].y = (imageInfo.y - imageInfo.ry) / GRID_UNIT;
	cell[4].z = (imageInfo.z - imageInfo.rz) / GRID_UNIT;

	cell[5].x = (imageInfo.x - imageInfo.rx) / GRID_UNIT;
	cell[5].y = (imageInfo.y - imageInfo.ry) / GRID_UNIT;
	cell[5].z = (imageInfo.z + imageInfo.rz) / GRID_UNIT;

	cell[6].x = (imageInfo.x + imageInfo.rx) / GRID_UNIT;
	cell[6].y = (imageInfo.y - imageInfo.ry) / GRID_UNIT;
	cell[6].z = (imageInfo.z + imageInfo.rz) / GRID_UNIT;

	cell[7].x = (imageInfo.x + imageInfo.rx) / GRID_UNIT;
	cell[7].y = (imageInfo.y - imageInfo.ry) / GRID_UNIT;
	cell[7].z = (imageInfo.z - imageInfo.rz) / GRID_UNIT;

	//determine if the object is split between cells
	if (cell[0].x != cell[6].x)
		xSplit = true;
	if (cell[0].y != cell[6].y)
		ySplit = true;
	if (cell[0].z != cell[6].z)
		zSplit = true;

	//add cell 0 as first gridCell
	gridCell.x = cell[0].x;
	gridCell.y = cell[0].y;
	gridCell.z = cell[0].z;

	//based on split create our cell list
	if (xSplit && ySplit && zSplit)
	{
		GridCell *tempCell = &gridCell;

		for (int i = 1; i <= 7; i++)
		{
			tempCell->next = new GridCell();
			tempCell->next->x = cell[i].x;
			tempCell->next->y = cell[i].y;
			tempCell->next->z = cell[i].z;
			tempCell = tempCell->next;
		}
	}
	else if (xSplit && ySplit)
	{
		GridCell *tempCell = &gridCell;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[3].x;
		tempCell->next->y = cell[3].y;
		tempCell->next->z = cell[3].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[4].x;
		tempCell->next->y = cell[4].y;
		tempCell->next->z = cell[4].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[7].x;
		tempCell->next->y = cell[7].y;
		tempCell->next->z = cell[7].z;
		tempCell = tempCell->next;
	}
	else if (xSplit && zSplit)
	{
		GridCell *tempCell = &gridCell;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[1].x;
		tempCell->next->y = cell[1].y;
		tempCell->next->z = cell[1].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[2].x;
		tempCell->next->y = cell[2].y;
		tempCell->next->z = cell[2].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[3].x;
		tempCell->next->y = cell[3].y;
		tempCell->next->z = cell[3].z;
		tempCell = tempCell->next;
	}
	else if (ySplit && zSplit)
	{
		GridCell *tempCell = &gridCell;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[2].x;
		tempCell->next->y = cell[2].y;
		tempCell->next->z = cell[2].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[6].x;
		tempCell->next->y = cell[6].y;
		tempCell->next->z = cell[6].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[7].x;
		tempCell->next->y = cell[7].y;
		tempCell->next->z = cell[7].z;
		tempCell = tempCell->next;
	}
	else if (xSplit || ySplit || zSplit)
	{
		gridCell.next = new GridCell();

		gridCell.next->x = cell[6].x;
		gridCell.next->y = cell[6].y;
		gridCell.next->z = cell[6].z;
	}

	//add new entries to grid collision table
	setGridLoc();

	GridCell *tempCell = &gridCell;
	
	for (int index = 0; index <= 7; index++)
	{
		if (gridLoc[index] == -1)
			break;
		
		GridContents *tempCollisionContents = &collisionGrid[gridLoc[index]];

		while (tempCollisionContents->gridCell.x != tempCell->x ||
			   tempCollisionContents->gridCell.y != tempCell->y ||
			   tempCollisionContents->gridCell.z != tempCell->z)
		{
			if (tempCollisionContents->next != NULL)
				tempCollisionContents = tempCollisionContents->next;
			else
				throw(GameError(gameErrorNS::FATAL_ERROR, L"Error with grid collision system - error adding object contents mismatch."));
		}
		//add to object list
		if (collisionGrid[gridLoc[index]].object == NULL)
		{//no objects
			collisionGrid[gridLoc[index]].object = new ObjectPtrList(this);
		}
		else
		{//already objects
			ObjectPtrList *tempObject = tempCollisionContents->object;
			while (tempObject->next != NULL)
				tempObject = tempObject->next;

			tempObject->next = new ObjectPtrList(this);
		}
		if (tempCell->next != NULL)
			tempCell = tempCell->next;
	}
}

//set collision grid array index
void Object::setGridLoc()
{
	return;
	int index = 0;
	GridCell *tempCell = &gridCell;

	while (tempCell != NULL)
	{
		gridLoc[index] = (HASH_XM * tempCell->x + HASH_YM * tempCell->y + HASH_ZM * tempCell->z) % GRID_HASH_SIZE;
		if (gridLoc[index] < 0) //make it so this doesn't happen
			gridLoc[index] = -gridLoc[index];//GRID_HASH_SIZE;
		index++;
		tempCell = tempCell->next;
	}
	for (index; index <= 7; index++)
	{
		gridLoc[index] = -1;
	}
}

void Object::setInitialGridCells()
{
	return;
	GridCell cell[8];
	bool xSplit = false;
	bool ySplit = false;
	bool zSplit = false;

	//compute cells of the objects 8 corners
	cell[0].x = (imageInfo.x - imageInfo.rx) / GRID_UNIT;
	cell[0].y = (imageInfo.y + imageInfo.ry) / GRID_UNIT;
	cell[0].z = (imageInfo.z - imageInfo.rz) / GRID_UNIT;

	cell[1].x = (imageInfo.x - imageInfo.rx) / GRID_UNIT;
	cell[1].y = (imageInfo.y + imageInfo.ry) / GRID_UNIT;
	cell[1].z = (imageInfo.z + imageInfo.rz) / GRID_UNIT;

	cell[2].x = (imageInfo.x + imageInfo.rx) / GRID_UNIT;
	cell[2].y = (imageInfo.y + imageInfo.ry) / GRID_UNIT;
	cell[2].z = (imageInfo.z + imageInfo.rz) / GRID_UNIT;

	cell[3].x = (imageInfo.x + imageInfo.rx) / GRID_UNIT;
	cell[3].y = (imageInfo.y + imageInfo.ry) / GRID_UNIT;
	cell[3].z = (imageInfo.z - imageInfo.rz) / GRID_UNIT;

	cell[4].x = (imageInfo.x - imageInfo.rx) / GRID_UNIT;
	cell[4].y = (imageInfo.y - imageInfo.ry) / GRID_UNIT;
	cell[4].z = (imageInfo.z - imageInfo.rz) / GRID_UNIT;

	cell[5].x = (imageInfo.x - imageInfo.rx) / GRID_UNIT;
	cell[5].y = (imageInfo.y - imageInfo.ry) / GRID_UNIT;
	cell[5].z = (imageInfo.z + imageInfo.rz) / GRID_UNIT;

	cell[6].x = (imageInfo.x + imageInfo.rx) / GRID_UNIT;
	cell[6].y = (imageInfo.y - imageInfo.ry) / GRID_UNIT;
	cell[6].z = (imageInfo.z + imageInfo.rz) / GRID_UNIT;

	cell[7].x = (imageInfo.x + imageInfo.rx) / GRID_UNIT;
	cell[7].y = (imageInfo.y - imageInfo.ry) / GRID_UNIT;
	cell[7].z = (imageInfo.z - imageInfo.rz) / GRID_UNIT;

	//determine if the object is split between cells
	if (cell[0].x != cell[6].x)
		xSplit = true;
	if (cell[0].y != cell[6].y)
		ySplit = true;
	if (cell[0].z != cell[6].z)
		zSplit = true;

	//add cell 0 as first gridCell
	gridCell.x = cell[0].x;
	gridCell.y = cell[0].y;
	gridCell.z = cell[0].z;

	//based on split create our cell list
	if (xSplit && ySplit && zSplit)
	{
		GridCell *tempCell = &gridCell;

		for (int i = 1; i <= 7; i++)
		{
			tempCell->next = new GridCell();
			tempCell->next->x = cell[i].x;
			tempCell->next->y = cell[i].y;
			tempCell->next->z = cell[i].z;
			tempCell = tempCell->next;
		}
	}
	else if (xSplit && ySplit)
	{
		GridCell *tempCell = &gridCell;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[3].x;
		tempCell->next->y = cell[3].y;
		tempCell->next->z = cell[3].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[4].x;
		tempCell->next->y = cell[4].y;
		tempCell->next->z = cell[4].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[7].x;
		tempCell->next->y = cell[7].y;
		tempCell->next->z = cell[7].z;
		tempCell = tempCell->next;
	}
	else if (xSplit && zSplit)
	{
		GridCell *tempCell = &gridCell;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[1].x;
		tempCell->next->y = cell[1].y;
		tempCell->next->z = cell[1].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[2].x;
		tempCell->next->y = cell[2].y;
		tempCell->next->z = cell[2].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[3].x;
		tempCell->next->y = cell[3].y;
		tempCell->next->z = cell[3].z;
		tempCell = tempCell->next;
	}
	else if (ySplit && zSplit)
	{
		GridCell *tempCell = &gridCell;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[2].x;
		tempCell->next->y = cell[2].y;
		tempCell->next->z = cell[2].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[6].x;
		tempCell->next->y = cell[6].y;
		tempCell->next->z = cell[6].z;
		tempCell = tempCell->next;

		tempCell->next = new GridCell();
		tempCell->next->x = cell[7].x;
		tempCell->next->y = cell[7].y;
		tempCell->next->z = cell[7].z;
		tempCell = tempCell->next;
	}
	else if (xSplit || ySplit || zSplit)
	{
		gridCell.next = new GridCell();

		gridCell.next->x = cell[6].x;
		gridCell.next->y = cell[6].y;
		gridCell.next->z = cell[6].z;
	}

	//add new entries to grid collision table
	setGridLoc();

	GridCell *tempCell = &gridCell;

	for (int index = 0; index <= 7; index++)
	{
		if (gridLoc[index] == -1)
			break;

		GridContents *tempCollisionContents = &collisionGrid[gridLoc[index]];

		while (tempCollisionContents->gridCell.x != tempCell->x ||
			tempCollisionContents->gridCell.y != tempCell->y ||
			tempCollisionContents->gridCell.z != tempCell->z)
		{
			if (tempCollisionContents->next != NULL)
				tempCollisionContents = tempCollisionContents->next;
			else
				throw(GameError(gameErrorNS::FATAL_ERROR, L"Error with grid collision system - error adding initial object contents mismatch."));
		}
		//add to object list
		if (collisionGrid[gridLoc[index]].object == NULL)
		{//no objects
			collisionGrid[gridLoc[index]].object = new ObjectPtrList(this);
		}
		else
		{//already objects
			ObjectPtrList *tempObject = tempCollisionContents->object;
			while (tempObject->next != NULL)
				tempObject = tempObject->next;

			tempObject->next = new ObjectPtrList(this);
		}
		if (tempCell->next != NULL)
			tempCell = tempCell->next;
	}
}

bool Object::collidesWith(Object *obj, DirectX::XMFLOAT3 *collisionVector)
{
	if (collisionType == ObjectNS::AABB && obj->collisionType == ObjectNS::AABB)
		return collideAABoundingBox(obj, collisionVector);
	else
		//base coverage in case of a problem
		return collideAABoundingBox(obj, collisionVector);
}

//switch check to check temporary movement values
/*bool Object::collideAABoundingBox(Object *obj, DirectX::XMFLOAT3 *collisionVector)
{
	if (imageInfo.x + imageInfo.rx < obj->imageInfo.x - obj->imageInfo.rx || imageInfo.x - imageInfo.rx > obj->imageInfo.x + obj->imageInfo.rx)
		return false;
	if (imageInfo.y + imageInfo.ry < obj->imageInfo.y - obj->imageInfo.ry || imageInfo.y - imageInfo.ry > obj->imageInfo.y + obj->imageInfo.ry)
		return false;
	if (imageInfo.z + imageInfo.rz < obj->imageInfo.z - obj->imageInfo.rz || imageInfo.z - imageInfo.rz > obj->imageInfo.z + obj->imageInfo.rz)
		return false;

	collisionVector->x = obj->imageInfo.x - imageInfo.x;
	collisionVector->y = obj->imageInfo.y - imageInfo.y;
	collisionVector->z = obj->imageInfo.z - imageInfo.z;
	return true;
}*/

bool Object::collideAABoundingBox(Object *obj, DirectX::XMFLOAT3 *collisionVector)
{
/*	if (tempX + imageInfo.rx < obj->tempX - obj->imageInfo.rx || tempX - imageInfo.rx > obj->tempX + obj->imageInfo.rx)
		return false;
	if (tempY + imageInfo.ry < obj->tempY - obj->imageInfo.ry || tempY - imageInfo.ry > obj->tempY + obj->imageInfo.ry)
		return false;
	if (tempZ + imageInfo.rz < obj->tempZ - obj->imageInfo.rz || tempZ - imageInfo.rz > obj->tempZ + obj->imageInfo.rz)
		return false;

	collisionVector->x = obj->tempX - tempX;
	collisionVector->y = obj->tempY - tempY;
	collisionVector->z = obj->tempZ - tempZ;*/
	if (imageInfo.x + imageInfo.rx < obj->imageInfo.x - obj->imageInfo.rx ||
		imageInfo.x - imageInfo.rx > obj->imageInfo.x + obj->imageInfo.rx)
		return false;
	else
	{//set collision vector x, positive x collision gives a negative value
		if (imageInfo.x + imageInfo.rx > obj->imageInfo.x - obj->imageInfo.rx)
			collisionVector->x = 0 - (imageInfo.x + imageInfo.rx - obj->imageInfo.x - obj->imageInfo.rx);
		else
			collisionVector->x = 0 + (obj->imageInfo.x + obj->imageInfo.rx - imageInfo.x - imageInfo.rx);
	}
	if (imageInfo.y + imageInfo.ry < obj->imageInfo.y - obj->imageInfo.ry || 
		imageInfo.y - imageInfo.ry > obj->imageInfo.y + obj->imageInfo.ry)
		return false;
	else
	{//set collision vector y, positive y collision gives a negative value
		if (imageInfo.y + imageInfo.ry > obj->imageInfo.y - obj->imageInfo.ry)
			collisionVector->y = 0 - (imageInfo.y + imageInfo.ry - obj->imageInfo.y - obj->imageInfo.ry);
		else
			collisionVector->y = 0 + (obj->imageInfo.y + obj->imageInfo.ry - imageInfo.y - imageInfo.ry);
	}
	if (imageInfo.z + imageInfo.rz < obj->imageInfo.z - obj->imageInfo.rz || 
		imageInfo.z - imageInfo.rz > obj->imageInfo.z + obj->imageInfo.rz)
		return false;
	else
	{//set collision vector z, positive z collision gives a negative value
		if (imageInfo.z + imageInfo.rz > obj->imageInfo.z - obj->imageInfo.rz)
			collisionVector->z = 0 - (imageInfo.z + imageInfo.rz - obj->imageInfo.z - obj->imageInfo.rz);
		else
			collisionVector->z = 0 + (obj->imageInfo.z + obj->imageInfo.rz - imageInfo.z - imageInfo.rz);
	}

	return true;
}

bool Object::collideOBoundingBox(Object *obj, DirectX::XMFLOAT3 *collisionVector)
{
	computeRotatedBox();
	obj->computeRotatedBox();

	return true;
}

void Object::computeRotatedBox()
{
	DirectX::XMFLOAT3 rotatedX();
	DirectX::XMFLOAT3 rotatedY();
	DirectX::XMFLOAT3 rotatedZ();
}

void Object::createPrimaryBoundingBox()
{
	if (collisionType == ObjectNS::AABB)
	{
		aabb.min.x = imageInfo.x - imageInfo.rx;
		aabb.min.y = imageInfo.y - imageInfo.ry;
		aabb.min.z = imageInfo.z - imageInfo.rz;

		aabb.max.x = imageInfo.x + imageInfo.rx;
		aabb.max.y = imageInfo.y + imageInfo.ry;
		aabb.max.z = imageInfo.z + imageInfo.rz;
	}
	else if (collisionType == ObjectNS::OBB)
	{
		return;
	}
	else if (collisionType == ObjectNS::SPHERE)
	{
		bs.radius = imageInfo.rx;
	}
}