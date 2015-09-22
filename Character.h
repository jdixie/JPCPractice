#ifndef _CHARACTER_H
#define _CHARACTER_H
#define WIN32_LEAN_AND_MEAN

#include "Object.h"

//character status bit indicators. short size will allow for 16 if we need that many.
namespace CharacterNS
{
	enum FACING { LEFT_FACE, RIGHT_FACE, UP_FACE, DOWN_FACE, OMNI_FACE };

	//movement bits
	const unsigned char LEFT = 1;
	const unsigned char RIGHT = 2;
	const unsigned char UP = 4;
	const unsigned char DOWN = 8;
	const unsigned char FORWARD = 16;
	const unsigned char BACKWARD = 32;

	//status bits
	const unsigned short JUMPING = 1;
	const unsigned short FALLING = 2;
	const unsigned short ATTACKING = 4;
	const unsigned short INVULNERABLE = 8;
	const unsigned short KNOCKEDBACK = 16;
}

class Character : public Object
{
protected:
	//potentially add movement states here
	//effects vars tbd
	unsigned char moveDirection;
	unsigned short status;	//character status bits per CharacterNS
	short attackStrength;	//attack strength for damage claculations
	short attackForce;		//force of attack for knockback calculations
	float knockbackStrength; //force of knockback currently being suffered
	DirectX::XMFLOAT3 vCollisionVector;

public:
	Character();
	virtual ~Character();

	virtual void setAttackStrength(short n) { attackStrength = n; }
	virtual void setAttackForce(short n) { attackForce = n; }
	virtual void setKnockbackStrength(float n, DirectX::XMFLOAT3 v)
	{
		knockbackStrength = n;
		vCollisionVector = v;
		setStatusKnockedback(true);
	}

	virtual void setMoveDirection(char n) { moveDirection = n; }
	virtual char getMoveDirection() { return moveDirection; }

	virtual short getAttackStrength() { return attackStrength; }
	virtual short getAttackForce() { return attackForce; }
	virtual float getKnockbackStrength() { return knockbackStrength; }

	virtual bool checkStatus(unsigned short bits) { return status & bits; }
	virtual void setStatusJumping(bool b);
	virtual void setStatusFalling(bool b);
	virtual void setStatusAttacking(bool b);
	virtual void setStatusInvulnerable(bool b);
	virtual void setStatusKnockedback(bool b);
	virtual void resetStatus() { status = 0; }
};

#endif