#include "Player.h"

extern GridContents collisionGrid[GRID_HASH_SIZE];

Player::Player()
{
	invulnerabilityTimer = 0.0;
}

Player::~Player()
{

}

bool Player::initialize(Input *in, Graphics *g, int w, int h, float sc)
{
	input = in;
	graphics = g;
	width = w;
	height = h;
	imageInfo.scale = sc;

	return true;
}

void Player::update(float frameTime, float gameScale)
{
	moveDirection = 0;
	//set player to false in grid

	moveVector.x = imageInfo.x;
	moveVector.y = imageInfo.y;
	moveVector.z = imageInfo.z;

	//first check is if in middle of a knockback, player movement denied, knockback continues
	if (checkStatus(CharacterNS::KNOCKEDBACK))
	{
		velocity.x = frameTime * -vCollisionVector.x * knockbackStrength;
		velocity.y = frameTime * -vCollisionVector.y * knockbackStrength;
		velocity.z = frameTime * -vCollisionVector.z * knockbackStrength;

		knockbackStrength -= frameTime * PlayerNS::KB_DECELERATION;
		if (knockbackStrength <= 0)
		{
			knockbackStrength = 0;
			setStatusKnockedback(false);
		}

//		currentFrame = PLAYER_STANDING_FRAME;
//		setRect();
	}
	else
	{//not knocked back, proceed with movement processing
		//check for jumping or falling

		//if falling, continue to fall - add terminal velocity at some point
		if (checkStatus(CharacterNS::FALLING))
			velocity.y -= frameTime * ObjectNS::GRAVITY;

		//if jumping continue ascent through cap, then start falling
		if ((input->isKeyDown(JUMP_KEY) || input->getGamepadA(0)) && !checkStatus(CharacterNS::FALLING))
		{
			if (!checkStatus(CharacterNS::JUMPING))
			{
				jumpForce = PlayerNS::JUMP_FORCE;
				velocity.y += jumpForce;
				setStatusJumping(true);
			}
			else
			{
				jumpForce -= frameTime * ObjectNS::GRAVITY;
				velocity.y += frameTime * jumpForce;
				if (jumpForce < 0)
				{
					jumpForce = 0;
					setStatusFalling(true);
					setStatusJumping(false);
				}
			}
		}
		//if you were jumping, but now aren't, start falling
		else if ((!input->isKeyDown(JUMP_KEY) && !input->getGamepadA(0)) && checkStatus(CharacterNS::JUMPING))
		{
			velocity.y = 0;
			setStatusJumping(false);
			setStatusFalling(true);
		}


		//check movement and animate accordingly - x directions
		if ((input->isKeyDown(MOVE_X_NEG) || input->isKeyDown(MOVE_X_POS)) || (input->getGamepadThumbLX(0) != 0))
		{
			if (input->isKeyDown(MOVE_X_POS) || input->getGamepadThumbLX(0) > 0)
			{
//				flipHorizontal(false);
				if (velocity.x >= 0)
					velocity.x += frameTime * PlayerNS::BASE_ACCELERATION;     //add check for if opposite dir movement and cancel
				else
					velocity.x += frameTime * PlayerNS::BASE_DECELERATION;     //add check for if opposite dir movement and cancel
				if (velocity.x > PlayerNS::MAX_RUN_SPEED)
					velocity.x = PlayerNS::MAX_RUN_SPEED;
			}
			else if (input->isKeyDown(MOVE_X_NEG) || input->getGamepadThumbLX(0) < 0)
			{
//				flipHorizontal(true);
				if (velocity.x <= 0)
					velocity.x -= frameTime * PlayerNS::BASE_ACCELERATION;
				else
					velocity.x -= frameTime * PlayerNS::BASE_DECELERATION;
				if (velocity.x < -PlayerNS::MAX_RUN_SPEED)
					velocity.x = -PlayerNS::MAX_RUN_SPEED;
			}

			animTimer += frameTime;
			if (animTimer > frameDelay)
			{
				animTimer -= frameDelay;
/*				currentFrame++;
				if (currentFrame < runningStartFrame || currentFrame > runningEndFrame)
				{
					currentFrame = runningStartFrame;
				}*/
//				setRect();                      //set spriteData.rect
			}
		}
		//decelerate if need be on x-diection release
		else if (!input->isKeyDown(MOVE_X_NEG) && !input->isKeyDown(MOVE_X_POS) && input->getGamepadThumbLX(0) == 0)
		{
			if (velocity.x > 0)
			{
				velocity.x -= PlayerNS::BASE_DECELERATION * frameTime;
				if (velocity.x < 0)
					velocity.x = 0;
			}
			else if (velocity.x <  0)
			{
				velocity.x += PlayerNS::BASE_DECELERATION * frameTime;
				if (velocity.x > 0)
					velocity.x = 0;
			}

//			currentFrame = PLAYER_STANDING_FRAME;
//			setRect();
		}//finish x-direction checks


		//check for attacking
		if (input->getGamepadX(0) || input->getMouseLButton())
		{
			setStatusAttacking(true);

			if (powerMeter >= 10)
			{
				powerMeter -= frameTime * 30;
				if (powerMeter < 0)
					powerMeter = 0;
			}
			else
			{
				setStatusAttacking(false);
			}
		}
		else
		{
			setStatusAttacking(false);
			powerMeter += 10 * frameTime;
			if (powerMeter > 100)
				powerMeter = 100;
		}//end attacking

		
	}//end player input updating

	if (invulnerabilityTimer <= 0)
	{
		invulnerabilityTimer = 0;
		setStatusInvulnerable(false);
	}
	else
	{
		invulnerabilityTimer -= frameTime * 10000;
	}


	//set temp position
//	tempX = getX() + velocity.x * gameScale;
//	tempY = getY() + velocity.y * gameScale;
//	tempZ = getZ() + velocity.z * gameScale;
	
	//set moveVector
	moveVector.x = velocity.x;
	moveVector.y = velocity.y;
	moveVector.z = velocity.z;

	imageInfo.x = getX() + velocity.x; //* gameScale;
	imageInfo.y = getY() + velocity.y; //* gameScale;
	imageInfo.z = getZ() + velocity.z; //* gameScale;

	if (imageInfo.x < 0)
		imageInfo.x = 0;
	if (imageInfo.y < 0)
		imageInfo.y = 0;
	if (imageInfo.z < 0)
		imageInfo.z = 0;

	if (velocity.x != 0 || velocity.y != 0 || velocity.z != 0)
		setGridCells();
//	setGridLoc();

	//set player to true in grid table maybe

}

void Player::setGridCells()
{
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

	setGridLoc();

}

//set collision grid array index
void Player::setGridLoc()
{
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

void Player::setInitialGridCells()
{
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

	setGridLoc();
}