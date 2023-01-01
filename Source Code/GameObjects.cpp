// David Dorrington UoBGames, 2022
// SDL Game Object Classes 
// =======================================================

#include "GameObjects.h"


//--------------------------------------------------------------------------
void Character::moveXY(char directionSent)
{
	// Apply Velocity 
	switch (directionSent)
	{
	case 'u':if (abs(yVel) < speed) yVel -= force; break;
	case 'd':if (abs(yVel) < speed) yVel += force; break;

	case 'l':if (abs(xVel) < speed)	xVel -= force; break;
	case 'r':if (abs(xVel) < speed)	xVel += force; break;
	}
}

void  Character::keyRotateMove(char actionSent)
{	//angle is in degrees, Trig uses radians ( degrees x PI/180)
	switch (actionSent)
	{
	case 'l': rotationRate = -torque; break;
	case 'r': rotationRate = torque; break;

	case 'f':
		if (xVel == 0 && yVel == 0) // Set Speed if still
		{
			xVel += sin(angle * M_PI / 180) * speed;
			yVel -= cos(angle * M_PI / 180) * speed;
		}
		else if (abs(xVel) < speed && abs(yVel) < speed) // limit speed
		{
			xVel += sin(angle * M_PI / 180) * force;
			yVel -= cos(angle * M_PI / 180) * force;
		} break;

	case'b':
		if (abs(xVel) < speed && abs(yVel) < speed) // limit speed
		{
			xVel -= sin(angle * M_PI / 180) * force;
			yVel += cos(angle * M_PI / 180) * force;
		}break;
	}
}

void Character::applyDrag()
{
	// apply linear drag - set vel to zero once clearly less than 1 pixel speed 
	if (abs(xVel) > 0.1f) xVel *= friction; else xVel = 0;
	if (abs(yVel) > 0.1f) yVel *= friction; else yVel = 0;
}

void Character::updatePosition(float frameTimeSent)
{
	// Add the rotation rate to the angle and apply drag
	if (abs(rotationRate) > 10)	rotationRate *= friction;
	else rotationRate = 0;
	angle += rotationRate * frameTimeSent;

	// Add the current velocity to current position
	// round up /down to nearest pixel depending on direction
	if (xVel > 0) x += floor(xVel * frameTimeSent);
	else if (xVel < 0) x += ceil(xVel * frameTimeSent);
	if (yVel > 0) y += floor(yVel * frameTimeSent);
	else if (yVel < 0) y += ceil(yVel * frameTimeSent);
}

void Character::screenCrawl()
{
	if (x > SCREEN_WIDTH - SPRITE_SIZE)
	{
		x = SCREEN_WIDTH - SPRITE_SIZE;
		if (yVel < 0)
			y += SPRITE_SIZE;
		else
			y -= SPRITE_SIZE;
		xVel = -xVel;
	}
	if (x < 1)
	{
		x = 1;
		if (yVel < 0)
			y += SPRITE_SIZE;
		else
			y -= SPRITE_SIZE;
		xVel = -xVel;
	}

	if (y > SCREEN_HEIGHT - SPRITE_SIZE)
	{
		y = 512;
		yVel = -yVel;
	}

	if (y < 32)
	{
		y = 0;
		yVel = -yVel;
	}

}

//void Character::screenLimit()
//{	// Limit sprite to screen 
//	if (x > SCREEN_WIDTH - SPRITE_SIZE)
//	{
//		xVel = 0;
//		x = SCREEN_WIDTH - SPRITE_SIZE;
//	}
//	if (x < 0)
//	{
//		xVel = 0;
//		x = 0;
//	}
//	if (y > SCREEN_HEIGHT - SPRITE_SIZE)
//	{
//		y = SCREEN_HEIGHT - SPRITE_SIZE;
//		yVel = 0;
//	}
//	if (y < 0)
//	{
//		y = 0;
//		yVel = 0;
//	}
//
//}

void Character::screenLimit()
{
	if (x > 736 - SPRITE_SIZE) 
	{
		xVel = 0;
		x = 736 - SPRITE_SIZE;
	}

	if (x < 96)
	{
		xVel = 0;
		x = 96;
	}

	if (y < 96)
	{
		yVel = 0;
		y = 96;
	}
	if (y > 512)
	{
		yVel = 0;
		y = 512;
	}
}

void Character::screenWrap()
{	// Screen Wrap to opposite side if sprite leaves screen
	if (x > SCREEN_WIDTH - SPRITE_SIZE)  x = 0;
	if (x < 0) x = SCREEN_WIDTH - SPRITE_SIZE;
	if (y > SCREEN_HEIGHT - SPRITE_SIZE) y = 0;
	if (y < 0) y = SCREEN_HEIGHT - SPRITE_SIZE;
}

void Character::screenBounce()
{ // reverse vel on edge hit
	if (x > SCREEN_WIDTH - SPRITE_SIZE)  xVel = -xVel;
	if (x < 0) xVel = -xVel;
	if (y > SCREEN_HEIGHT - SPRITE_SIZE) yVel = -yVel;
	if (y < 0) yVel = -yVel;
}

void Character::mouseMove(int newX, int newY)
{	// chase the cursor
	if (newX > x) x += SPRITE_SIZE;
	if (newX < x) x -= SPRITE_SIZE;
	if (newY > y) y += SPRITE_SIZE;
	if (newY < y) y -= SPRITE_SIZE;
}

void Character::rotate(float frameTimeSent)
{
	angle += rotationRate * frameTimeSent;
}

void Character::setVelocity(float velSent)
{
	xVel = sin(angle * M_PI / 180) * velSent;
	yVel = -cos(angle * M_PI / 180) * velSent;
}

void Character::applyGravity()
{
	yVel += 10 * friction;
}

void Character::jump()
{
	yVel = -500;
}

//--------------------------------------------------------------------------

void Projectile::updatePosition(float frameTimeSent)
{
	if (xVel > 0) x += floor(xVel * frameTimeSent);
	else if (xVel < 0) x += ceil(xVel * frameTimeSent);
	if (yVel > 0) y += floor(yVel * frameTimeSent);
	else if (yVel < 0) y += ceil(yVel * frameTimeSent);
}

void Projectile::checkIsOnScreen()
{
	if ((x > SCREEN_WIDTH) || x < 0 || (y > SCREEN_HEIGHT) || y < 0)
		isActive = false;
}

void Projectile::fire(float xSent, float ySent, float angleSent)
{
	if (!isActive)
	{
		isActive = true;
		//Mix_PlayChannel(-1, shootSound, 0);
		x = xSent + 16;
		y = ySent + 16;
		angle = angleSent;
		xVel = sin(angle * M_PI / 180) * speed;
		yVel = -cos(angle * M_PI / 180) * speed;
	}
}

//--------------------------------------------------------------------------
void NPC::patrol(int w1x, int w1y, int w2x, int w2y, int w3x, int w3y, int w4x, int w4y)
{
	wp1x = w1x;
	wp2x = w2x;
	wp3x = w3x;
	wp4x = w4x;

	wp1y = w1y;
	wp2y = w2y;
	wp3y = w3y;
	wp4y = w4y;

	currentWP = 1;
}

void NPC::update(int targetX, int targetY, int stateSent)
{
	if (stateSent == 1)
	{
		xVel = 0;
		yVel = 0;
	}
	if (stateSent == 2)
	{
		// random heading
		// until hit some thing
		x++;
		xVel = 0;
		yVel = 0;
	}
	if (stateSent == 3) //chase
	{
		if (x > targetX) x--;
		if (x < targetX) x++;

		if (y > targetY) y--;
		if (y < targetY) y++;
	}
	if (stateSent == 4) //evade
	{
		if (x > targetX) x++;
		if (x < targetX) x--;

		if (y > targetY) y++;
		if (y < targetY) y--;
	}
	if (stateSent == 5) //waypoints
	{

		if (currentWP == 1) //move to waypoint 1
		{
			if (x > wp1x) x--;
			if (x < wp1x) x++;
			if (y > wp1y) y--;
			if (y < wp1y) y++;
			if (x == wp1x && y == wp1y)  currentWP = 2;
		}
		if (currentWP == 2) //move to waypoint 2
		{
			if (x > wp2x) x--;
			if (x < wp2x) x++;

			if (y > wp2y) y--;
			if (y < wp2y) y++;
			if (x == wp2x && y == wp2y)  currentWP = 3;
		}
		if (currentWP == 3) //waypoint 3
		{
			if (x > wp3x) x--;
			if (x < wp3x) x++;

			if (y > wp3y) y--;
			if (y < wp3y) y++;
			if (x == wp3x && y == wp3y)  currentWP = 4;
		}
		if (currentWP == 4) //waypoint 4
		{
			if (x > wp4x) x--;
			if (x < wp4x) x++;

			if (y > wp4y) y--;
			if (y < wp4y) y++;
			if (x == wp4x && y == wp4y)  currentWP = 1;
		}
	}
	if (stateSent == 6) //random movement
	{
		// Move NPCs Random
		{
		
		}
	}

	// Limit sprite to screen 
	/*if (x > SCREEN_WIDTH - SPRITE_SIZE)
	{
		xVel = 0;
		x = SCREEN_WIDTH - SPRITE_SIZE;
	}
	if (x < 0)
	{
		xVel = 0;
		x = 0;
	}
	if (y > SCREEN_HEIGHT - SPRITE_SIZE)
	{
		y = SCREEN_HEIGHT - SPRITE_SIZE;
		yVel = 0;
	}
	if (y < 0)
	{
		y = 0;
		yVel = 0;
	}*/

	//limit the sprites to the screen
	if (x > 736 - SPRITE_SIZE)
	{
		xVel = 0;
		x = 736 - SPRITE_SIZE;
	}

	if (x < 96)
	{
		xVel = 0;
		x = 96;
	}

	if (y < 96)
	{
		yVel = 0;
		y = 96;
	}
	if (y > 512)
	{
		yVel = 0;
		y = 512;
	}
}
//--------------------------------------------------------------------------

// =======================================================
