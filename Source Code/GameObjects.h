#pragma once
#include <stdio.h>
#include<iostream>
#include <SDL.h>
#include<string>

// Constants
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define SPRITE_SIZE 32
#define FPS 50



// =======================================================
// classes

class Object
{
public:
	bool isActive = false;
	int x, y;
	SDL_Texture* spriteTexture = nullptr;
};//-----

class Character
{
public:
	bool isAlive = true;
	int lives = 3;
	int x = 0, y = 0;
	float xVel = 0, yVel = 0;
	float friction = 0.95f;
	float angle = 90;
	float speed = 200;
	float force = 150;
	float torque = 120; // rotation force
	float rotationRate = 0;
	SDL_Texture* spriteTexture = nullptr;

	// -----------------------

	void moveXY(char directionSent);
	void keyRotateMove(char actionSent);
	void applyDrag();
	void updatePosition(float frameTimeSent);
	void screenCrawl();
	void screenLimit();
	void screenWrap();
	void screenBounce();
	void mouseMove(int newX, int newY);
	void rotate(float frameTimeSent);
	void setVelocity(float velSent);
	void applyGravity();
	void jump();

}; // End Char Class -------------------------------------

class Projectile
{
public:
	bool isActive = false;
	Uint32 lastTimeActivated = 0; // for the bullet array
	int size = 8;
	int x = 0, y = 0;
	float speed = 500;
	float xVel = 0, yVel = 0;
	float angle = 45;
	SDL_Texture* spriteTexture = nullptr;

	// -----------------------

	void updatePosition(float frameTimeSent);
	void checkIsOnScreen();
	void fire(float xSent, float ySent, float angleSent);

}; // End Projectile Class

class NPC
{
public:
	bool isAlive = false;
	//string state;
	SDL_Texture* spriteTexture = nullptr;

	int x = 0, y = 0;
	int health = 1;
	float xVel = 0, yVel = 0;
	float angle = 0;
	float speed = 400;
	int wp1x, wp2x, wp3x, wp4x;
	int wp1y, wp2y, wp3y, wp4y;
	int currentWP;

	// -----------------------

	void patrol(int w1x, int w1y, int w2x, int w2y, int w3x, int w3y, int w4x, int w4y);
	void update(int targetX, int targetY, int stateSent);

};// End Class -----

//npc for level 2
class NPC2: public NPC
{
public:
	int health = 2;
};

class NPC3 : public NPC
{
public:
	float speed = 600;
	int health = 3;
};
