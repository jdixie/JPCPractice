#ifndef _PLAYER_H
#define _PLAYER_H
#define WIN32_LEAN_AND_MEAN

#include "Character.h"
#include "input.h"

namespace PlayerNS
{
	enum ANIMATE_TYPE {
		NONE, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, JUMP_LEFT, JUMP_RIGHT,
		ATTACK_LEFT, ATTACK_RIGHT
	};
	const float BASE_ACCELERATION = 10;
	const float MAX_RUN_SPEED = 10;
	const float JUMP_FORCE = 16;
	const float BASE_DECELERATION = 10.5 * BASE_ACCELERATION;
	const float KB_DECELERATION = 20;
	const float TERMINAL_VELOCITY = 1000;
}

class Player : public Character
{
protected:
	Input *input;
	float invulnerabilityTimer;		//to keep track of any invulnerability countdowns
									//switch it to use timer class at some point when it's time

	unsigned char tempMove;

public:
	Player();
	~Player();

	bool initialize(Input *in, Graphics *g, int w, int h, float sc);
	void update(float frameTime, float gameScale);
	
	//set collision grid cell
	virtual void setGridCells();
	virtual void setInitialGridCells();

	//set collision grid array index
	virtual void setGridLoc();
};

#endif