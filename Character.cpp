#include "Character.h"

extern GridContents collisionGrid[GRID_HASH_SIZE];

Character::Character() : Object()
{
	status = 0;
	knockbackStrength = 0;	//when knockedback, this will keep track of how far to still be moved based on the knockback value
}

Character::~Character()
{
	Object::~Object();
}

void Character::setStatusJumping(bool b)
{
	if (b)
		status = status | CharacterNS::JUMPING;
	else
		status = status & 65534;
}

void Character::setStatusFalling(bool b)
{
	if (b)
		status = status | CharacterNS::FALLING;
	else
		status = status & 65533;
}

void Character::setStatusAttacking(bool b)
{
	if (b)
		status = status | CharacterNS::ATTACKING;
	else
		status = status & 65531;
}

void Character::setStatusInvulnerable(bool b)
{
	if (b)
		status = status | CharacterNS::INVULNERABLE;
	else
		status = status & 65527;
}

void Character::setStatusKnockedback(bool b)
{
	if (b)
		status = status | CharacterNS::KNOCKEDBACK;
	else
		status = status & 65519;
}