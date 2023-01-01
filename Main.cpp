// =======================================================
// SDL Game: Pig Elimination Sim
// Cameron Crook UoBGames, May 2022
// =======================================================





// Include Elements
#include "GameObjects.h"
#include "Levels.h"
#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h> 
#include <stdio.h>
#include<iostream>
using namespace std;
#include<math.h>
#include<string>

//audio
#include <SDL_mixer.h>

// Function Prototypes 
void startSDL();
void welcomeScreen();
void createGameSprites();
void createLevel();
void playerInputHandler();
void checkCollision();
void updateGameObjects();
void checkGameStates();
void updateGUI();
void updateScreen();
void LevelCompleteScreen();
void limitFrameRate(Uint32);
void closeSDL();
void checkCompletion();
void restartScreen();

// Game Constants - relaced before compiling
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPRITE_SIZE 32
#define FPS 50

// =======================================================
//Global Variables
float frameTime;
SDL_Window* gameWindow = NULL;
SDL_Surface* mainGameSurface = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* gameBackground = NULL;
SDL_Event playerInputEvent;

// Gobal Text Variables
TTF_Font* font = nullptr;
SDL_Color textColour = { 255, 255, 255 };
SDL_Surface* textSurface = nullptr;
SDL_Texture* textTexture = nullptr;
SDL_Rect textRect;

//AUDIO variables
Mix_Music* music = NULL;
Mix_Chunk* shootSound = NULL;
Mix_Chunk* destroySound = NULL;
Mix_Chunk* enemyDeathSound = NULL;
Mix_Chunk* playerHitSound = NULL;
Mix_Chunk* healSound = NULL;


//// game Objects
Character pc;
Projectile bullet;
Object terrainBlock[100];
Object backGroundBlocks[352];
NPC enemies[15];
NPC2 enemies2[15]; //more health
NPC3 enemies3[15]; //more speed
Object waypoints[4];
Object sceneryBlocks[40];
Object crateBlocks[20];
Object eggs[5];

//Game Play
bool gamePlaying = true;
bool replay = true;
int npcState = 1;
int score = 0;
bool goalReached = false;
bool quit = false;

int currentLevel = 1;
int gameLevels = 3;
int enemyDefeated = 0;
int close = 1;

// =======================================================
int main(int argc, char* args[])
{
	Uint32 frameStartTime;

	startSDL();
	welcomeScreen();

	createGameSprites();
	Mix_PlayMusic(music, -1);

	//Main Game Loop  
	while (replay)
	{
		createLevel();


		while (gamePlaying)
		{
			frameStartTime = SDL_GetTicks(); // time since starting in milliseconds

			playerInputHandler();
			updateGameObjects();
			checkGameStates();
			checkCompletion();
			updateScreen();


			limitFrameRate(frameStartTime);
		}

		LevelCompleteScreen();
	}
	closeSDL();
	return 0;
}//-----



// =======================================================

int xStart = 64;
int yStart = 64;

void createLevel()
{

	enemyDefeated = 0;
	// Level current level
	int currentMap[16][22];

	if (currentLevel == 1)
	{
		pc.lives = 3;
		for (int row = 0; row < 16; row++)
		{
			for (int col = 0; col < 22; col++)
			{
				currentMap[row][col] = level1[row][col];
			}
		}
	}
	if (currentLevel == 2)
	{
		for (int row = 0; row < 16; row++)
		{
			for (int col = 0; col < 22; col++)
			{
				currentMap[row][col] = level2[row][col];
			}
		}
	}

	if (currentLevel == 3)
	{
		for (int row = 0; row < 16; row++)
		{
			for (int col = 0; col < 22; col++)
			{
				currentMap[row][col] = level3[row][col];
			}
		}
	}

	// Add Game Objects to the board ------------------------

	for (int row = 0; row < 16; row++)
	{
		for (int col = 0; col < 22; col++)
		{
			if (currentMap[row][col] == 0)
			{
				//Empty
			}

			if (currentMap[row][col] == 1) //pc start pos
			{
				for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++) //do background first
				{
					if (backGroundBlocks[i].isActive == false)
					{
						backGroundBlocks[i].x = xStart + col * SPRITE_SIZE;
						backGroundBlocks[i].y = yStart + row * SPRITE_SIZE;
						backGroundBlocks[i].isActive = true;
						break;
					}
				}

				// PC Start
				pc.x = xStart + col * SPRITE_SIZE;
				pc.y = yStart + row * SPRITE_SIZE;
				pc.angle = 0;
			}

			if (currentMap[row][col] == 2) // terrain
			{
				// loop through terrin
				for (int i = 0; i < sizeof(terrainBlock) / sizeof(terrainBlock[0]); i++)
				{
					if (terrainBlock[i].isActive == false)
					{
						terrainBlock[i].x = xStart + col * SPRITE_SIZE;
						terrainBlock[i].y = yStart + row * SPRITE_SIZE;
						terrainBlock[i].isActive = true;
						break;
					}
				}
			}

			if (currentMap[row][col] == 3) // Enemies
			{
				for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++) //do background first
				{
					if (backGroundBlocks[i].isActive == false)
					{
						backGroundBlocks[i].x = xStart + col * SPRITE_SIZE;
						backGroundBlocks[i].y = yStart + row * SPRITE_SIZE;
						backGroundBlocks[i].isActive = true;
						break;
					}
				}
				for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++) //put enemy on background
				{
					if (enemies[i].isAlive == false)
					{
						enemies[i].x = xStart + col * SPRITE_SIZE;
						enemies[i].y = yStart + row * SPRITE_SIZE;
						enemies[i].isAlive = true;
						break;
					}
					if (i % 2 == 0) {
						enemies[i].patrol(waypoints[0].x, waypoints[0].y, waypoints[1].x, waypoints[1].y,
							waypoints[3].x, waypoints[3].y, waypoints[2].x, waypoints[2].y);
					}
					else {
						enemies[i].patrol(waypoints[0].x, waypoints[0].y, waypoints[2].x, waypoints[2].y,
							waypoints[3].x, waypoints[3].y, waypoints[1].x, waypoints[1].y);

					}
					//for all enemies to patrol same path
					//enemies[i].patrol(waypoints[0].x, waypoints[0].y, waypoints[1].x, waypoints[1].y,
						//waypoints[3].x, waypoints[3].y, waypoints[2].x, waypoints[2].y);

				}
			}

			if (currentMap[row][col] == 4) //background non collidable
			{
				// loop through background blocks 
				for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++)
				{
					if (backGroundBlocks[i].isActive == false)
					{
						backGroundBlocks[i].x = xStart + col * SPRITE_SIZE;
						backGroundBlocks[i].y = yStart + row * SPRITE_SIZE;
						backGroundBlocks[i].isActive = true;
						break;
					}
				}
			}
			if (currentMap[row][col] == 5) //crates destroyable
			{

				//loop through backgroundBlocks first
				for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++) //do background first
				{
					if (backGroundBlocks[i].isActive == false)
					{
						backGroundBlocks[i].x = xStart + col * SPRITE_SIZE;
						backGroundBlocks[i].y = yStart + row * SPRITE_SIZE;
						backGroundBlocks[i].isActive = true;
						break;
					}
				}
				// loop through crates
				for (int i = 0; i < sizeof(crateBlocks) / sizeof(crateBlocks[0]); i++)
				{
					if (crateBlocks[i].isActive == false)
					{
						crateBlocks[i].x = xStart + col * SPRITE_SIZE;
						crateBlocks[i].y = yStart + row * SPRITE_SIZE;
						crateBlocks[i].isActive = true;
						break;
					}
				}
			}

			if (currentMap[row][col] == 6) //Scenerary - bushes
			{
				//loop through blocks first
				for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++) //do background first
				{
					if (backGroundBlocks[i].isActive == false)
					{
						backGroundBlocks[i].x = xStart + col * SPRITE_SIZE;
						backGroundBlocks[i].y = yStart + row * SPRITE_SIZE;
						backGroundBlocks[i].isActive = true;
						break;
					}
				}
				// loop through scenery
				for (int i = 0; i < sizeof(sceneryBlocks) / sizeof(sceneryBlocks[0]); i++)
				{
					if (sceneryBlocks[i].isActive == false)
					{
						sceneryBlocks[i].x = xStart + col * SPRITE_SIZE;
						sceneryBlocks[i].y = yStart + row * SPRITE_SIZE;
						sceneryBlocks[i].isActive = true;
						break;
					}
				}
			}

			if (currentMap[row][col] == 7) //waypoints
			{

				//do background first
				for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++) //do background first
				{
					if (backGroundBlocks[i].isActive == false)
					{
						backGroundBlocks[i].x = xStart + col * SPRITE_SIZE;
						backGroundBlocks[i].y = yStart + row * SPRITE_SIZE;
						backGroundBlocks[i].isActive = true;
						break;
					}
				}
				//wps
				for (int i = 0; i < sizeof(waypoints) / sizeof(waypoints[0]); i++)
				{
					if (waypoints[i].isActive == false)
					{
						waypoints[i].x = xStart + col * SPRITE_SIZE;
						waypoints[i].y = yStart + row * SPRITE_SIZE;
						waypoints[i].isActive = true;

						break;
					}
				}
			}
			if (currentMap[row][col] == 8) //eggs that increase health
			{
				//do background first
				for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++) //do background first
				{
					if (backGroundBlocks[i].isActive == false)
					{
						backGroundBlocks[i].x = xStart + col * SPRITE_SIZE;
						backGroundBlocks[i].y = yStart + row * SPRITE_SIZE;
						backGroundBlocks[i].isActive = true;
						break;
					}
				}

				// loop through eggs
				for (int i = 0; i < sizeof(eggs) / sizeof(eggs[0]); i++)
				{
					if (eggs[i].isActive == false)
					{
						eggs[i].x = xStart + col * SPRITE_SIZE;
						eggs[i].y = yStart + row * SPRITE_SIZE;
						eggs[i].isActive = true;
						break;
					}
				}
			}

			if (currentMap[row][col] == 9) //enemies2 
			{
				//do background first
				for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++) //do background first
				{
					if (backGroundBlocks[i].isActive == false)
					{
						backGroundBlocks[i].x = xStart + col * SPRITE_SIZE;
						backGroundBlocks[i].y = yStart + row * SPRITE_SIZE;
						backGroundBlocks[i].isActive = true;
						break;
					}
				}

				// loop through enemies
				for (int i = 0; i < sizeof(enemies2) / sizeof(enemies2[0]); i++)
				{
					if (eggs[i].isActive == false)
					{
						enemies2[i].x = xStart + col * SPRITE_SIZE;
						enemies2[i].y = yStart + row * SPRITE_SIZE;
						enemies2[i].isAlive = true;
						break;
					}
				}
			}
			if (currentMap[row][col] == 10) //enemies3
			{
				//do background first
				for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++) //do background first
				{
					if (backGroundBlocks[i].isActive == false)
					{
						backGroundBlocks[i].x = xStart + col * SPRITE_SIZE;
						backGroundBlocks[i].y = yStart + row * SPRITE_SIZE;
						backGroundBlocks[i].isActive = true;
						break;
					}
				}

				// loop through enemies 3
				for (int i = 0; i < sizeof(enemies3) / sizeof(enemies3[0]); i++) //do background first
				{
					if (enemies3[i].isAlive == false)
					{
						enemies3[i].x = xStart + col * SPRITE_SIZE;
						enemies3[i].y = yStart + row * SPRITE_SIZE;
						enemies3[i].isAlive = true;
						break;
					}
				}

			}



		}

	}//-----
}


// =======================================================
void checkCollision()
{
	SDL_Rect bulletRect = { bullet.x, bullet.y, bullet.size, bullet.size };
	SDL_Rect npcRect;
	SDL_Rect objectRect;
	SDL_Rect pcRect = { pc.x, pc.y, SPRITE_SIZE, SPRITE_SIZE };

	// Enemy1 hit by bullet
	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		if (bullet.isActive)
		{
			if (enemies[i].isAlive)
			{
				objectRect.x = enemies[i].x;
				objectRect.y = enemies[i].y;
				objectRect.h = objectRect.w = SPRITE_SIZE;

				if (SDL_HasIntersection(&bulletRect, &objectRect)) // Enemy Hit by Bullet
				{

					bullet.isActive = false;
					enemies[i].health -= 1;
					if (enemies[i].health < 1)
					{
						enemies[i].isAlive = false;
						Mix_PlayChannel(-1, enemyDeathSound, 0);
						enemyDefeated++;
					}
				}
			}
		}
	}

	//enemy2 hit by bullet
	for (int i = 0; i < sizeof(enemies2) / sizeof(enemies2[0]); i++)
	{
		if (bullet.isActive)
		{
			if (enemies2[i].isAlive)
			{
				objectRect.x = enemies2[i].x;
				objectRect.y = enemies2[i].y;
				objectRect.h = objectRect.w = SPRITE_SIZE;

				if (SDL_HasIntersection(&bulletRect, &objectRect)) // Enemy Hit by Bullet
				{

					bullet.isActive = false;
					enemies2[i].health -= 1;
					if (enemies2[i].health < 1)
					{
						enemies2[i].isAlive = false;
						Mix_PlayChannel(-1, enemyDeathSound, 0);
						enemyDefeated++;
					}
				}
			}
		}
	}

	//enemy 3 hit by bullet
	for (int i = 0; i < sizeof(enemies3) / sizeof(enemies3[0]); i++)
	{
		if (bullet.isActive)
		{
			if (enemies3[i].isAlive)
			{
				objectRect.x = enemies3[i].x;
				objectRect.y = enemies3[i].y;
				objectRect.h = objectRect.w = SPRITE_SIZE;

				if (SDL_HasIntersection(&bulletRect, &objectRect)) // Enemy Hit by Bullet
				{

					bullet.isActive = false;
					enemies3[i].health -= 1;
					if (enemies3[i].health < 1)
					{
						enemies3[i].isAlive = false;
						Mix_PlayChannel(-1, enemyDeathSound, 0);
						enemyDefeated++;
					}
				}
			}
		}
	}

	//terrain hit by bullet
	for (int i = 0; i < sizeof(terrainBlock) / sizeof(terrainBlock[0]); i++)
	{
		if (bullet.isActive)
		{


			objectRect.x = terrainBlock[i].x;
			objectRect.y = terrainBlock[i].y;
			objectRect.h = objectRect.w = SPRITE_SIZE;

			if (SDL_HasIntersection(&bulletRect, &objectRect)) // Terrain Hit by Bullet
			{
				bullet.isActive = false;
			}

		}


	}
	//bullet hits crate
	for (int i = 0; i < sizeof(crateBlocks) / sizeof(crateBlocks[0]); i++)
	{
		if (bullet.isActive)
		{
			if (crateBlocks[i].isActive)
			{
				objectRect.x = crateBlocks[i].x;
				objectRect.y = crateBlocks[i].y;
				objectRect.h = objectRect.w = SPRITE_SIZE;

				if (SDL_HasIntersection(&bulletRect, &objectRect)) // Terrain Hit by Bullet
				{
					Mix_PlayChannel(-1, destroySound, 0);
					bullet.isActive = false;
					crateBlocks[i].isActive = false;
				}
			}
		}


	}

	//player intersects with enemy1
	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		if (enemies[i].isAlive)
		{
			objectRect.x = enemies[i].x;
			objectRect.y = enemies[i].y;
			objectRect.h = objectRect.w = SPRITE_SIZE;

			if (SDL_HasIntersection(&pcRect, &objectRect)) // Player hits enemy
			{
				Mix_PlayChannel(-1, playerHitSound, 0);
				pc.lives -= 1;
				enemies[i].isAlive = false;
				enemyDefeated++;

			}
		}
	}

	//intersects with enemy 2
	for (int i = 0; i < sizeof(enemies2) / sizeof(enemies2[0]); i++)
	{
		if (enemies2[i].isAlive)
		{
			objectRect.x = enemies2[i].x;
			objectRect.y = enemies2[i].y;
			objectRect.h = objectRect.w = SPRITE_SIZE;

			if (SDL_HasIntersection(&pcRect, &objectRect)) // Player hits enemy
			{
				Mix_PlayChannel(-1, playerHitSound, 0);
				pc.lives -= 1;
				enemies2[i].isAlive = false;
				enemyDefeated++;
			}
		}
	}

	//intersects with enemy 3
	for (int i = 0; i < sizeof(enemies3) / sizeof(enemies3[0]); i++)
	{
		if (enemies3[i].isAlive)
		{
			objectRect.x = enemies3[i].x;
			objectRect.y = enemies3[i].y;
			objectRect.h = objectRect.w = SPRITE_SIZE;

			if (SDL_HasIntersection(&pcRect, &objectRect)) // Player hits enemy
			{
				Mix_PlayChannel(-1, playerHitSound, 0);
				pc.lives -= 2;
				enemies3[i].isAlive = false;
				enemyDefeated++;

			}
		}
	}

	//player intersects (eats) egg
	for (int i = 0; i < sizeof(eggs) / sizeof(eggs[0]); i++)
	{
		if (eggs[i].isActive)
		{


			objectRect.x = eggs[i].x;
			objectRect.y = eggs[i].y;
			objectRect.h = objectRect.w = SPRITE_SIZE;

			if (SDL_HasIntersection(&pcRect, &objectRect)) // Player eats egg
			{
				Mix_PlayChannel(-1, healSound, 0);
				pc.lives += 1; //increase the amount of lives by 1
				eggs[i].isActive = false;

			}

		}


	}


}//----

// =======================================================
void updateGameObjects()
{
	// Collision Detection ---------------- 
	checkCollision();


	//--------------------------------------
	// Player Character
	pc.screenLimit();
	pc.applyDrag();
	pc.updatePosition(frameTime);

	//// enemies 1
	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		if (enemies[i].isAlive)
		{
			enemies[i].update(pc.x, pc.y, npcState);
		}
	}

	//enemies 2
	for (int i = 0; i < sizeof(enemies2) / sizeof(enemies2[0]); i++)
	{
		if (enemies2[i].isAlive)
		{
			enemies2[i].update(pc.x, pc.y, npcState);
		}
	}

	//enemies 3
	for (int i = 0; i < sizeof(enemies3) / sizeof(enemies3[0]); i++)
	{
		if (enemies3[i].isAlive)
		{
			enemies3[i].update(pc.x, pc.y, npcState);
		}
	}


	//bullet
	if (bullet.isActive)
	{
		bullet.checkIsOnScreen();
		bullet.updatePosition(frameTime);
	}

}//-----

void checkGameStates()
{
	if (pc.lives < 1)
	{
		//make enemies de-active
		for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
		{
			if (enemies[i].isAlive) // only those that are still alive
			{
				enemies[i].isAlive = false; //make them disappear
			}
		}

		//enemies2 deactivate
		for (int i = 0; i < sizeof(enemies2) / sizeof(enemies2[0]); i++)
		{
			if (enemies2[i].isAlive) // only those that are still alive
			{
				enemies2[i].isAlive = false; //make them disappear
			}
		}
		//enemies 3 deactivate
		for (int i = 0; i < sizeof(enemies3) / sizeof(enemies3[0]); i++)
		{
			if (enemies3[i].isAlive) // only those that are still alive
			{
				enemies3[i].isAlive = false; //make them disappear
			}
		}

		//make crates disappear
		for (int i = 0; i < sizeof(crateBlocks) / sizeof(crateBlocks[0]); i++)
		{
			if (crateBlocks[i].isActive) // only those that are still alive
			{
				crateBlocks[i].isActive = false; //make them disappear
			}
		}

		currentLevel = 1; //reset level to 1
		enemyDefeated = 0; //reset num of defeated enemies



		gamePlaying = false; //restart game


	}

	if (currentLevel == 1)
	{
		npcState = 5;
		if (enemyDefeated >= 3)
		{
			npcState = 3;
		}
	}

	if (currentLevel == 2)
	{
		npcState = 3;
		if (enemyDefeated == 5)
		{
			npcState = 4;
		}
		if (close == 1)
		{
			// Load Level 2
			gamePlaying = false;

			//make sure this doesnt keep happening
			close = 2;
		}
	}


	if (currentLevel == 3 and close == 2)
	{
		// Load Level 3
		gamePlaying = false;
		close = 3;
	}


	if (goalReached)
	{
		// Load Win Screen
		const char* startImageFile = "images/win.png"; //when game quit, this is what appears!
		SDL_Surface* startImage = IMG_Load(startImageFile);

		// merge image with surface
		SDL_BlitSurface(startImage, NULL, mainGameSurface, NULL);

		SDL_UpdateWindowSurface(gameWindow);
		SDL_Delay(2000);
		SDL_FreeSurface(startImage);


		//player presses 'Q':
		//replay = false;
		//gamePlaying = false;

	}
}

void checkCompletion()
{
	if (currentLevel == 1 && enemyDefeated == 6) {
		currentLevel = 2; //change level
		enemyDefeated = 0; //resets num of enemy defeated
		//createLevel();
	}

	if (currentLevel == 2 && enemyDefeated == 6) {
		currentLevel = 3;
		enemyDefeated = 0;
		//createLevel();
	}

	if (currentLevel == 3 && enemyDefeated == 4) {
		goalReached = true;
	}
}

// =======================================================
void updateGUI()
{
	string  screenText;

	// PC lives
	screenText = " Lives " + std::to_string(pc.lives);

	textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	int textW = 0;
	int textH = 0;

	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 700, 8, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	//enemies defeated
	screenText = "Enemies Defeated: " + std::to_string(enemyDefeated);
	textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 300, 8, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	// Current Level  
	screenText = "Level: " + std::to_string(currentLevel);
	textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 100, 8, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}//-----


// =======================================================
void updateScreen()
{
	SDL_Rect spriteRect = { 0, 0, SCREEN_WIDTH , SCREEN_HEIGHT };
	// Clear & Update the screen
	SDL_RenderClear(renderer);
	// Set the Background Image
	SDL_RenderCopy(renderer, gameBackground, NULL, &spriteRect);

	// Terrain Blocks
	for (int i = 0; i < sizeof(terrainBlock) / sizeof(terrainBlock[0]); i++)
	{
		if (terrainBlock[i].isActive) // exlcude the dead 
		{
			spriteRect = { terrainBlock[i].x, terrainBlock[i].y, SPRITE_SIZE , SPRITE_SIZE };
			SDL_RenderCopyEx(renderer, terrainBlock[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	//background
	for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++)
	{
		if (backGroundBlocks[i].isActive) // exlcude the dead 
		{
			spriteRect = { backGroundBlocks[i].x, backGroundBlocks[i].y, SPRITE_SIZE , SPRITE_SIZE };
			SDL_RenderCopyEx(renderer, backGroundBlocks[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	//uncollidable objects - bushes

	for (int i = 0; i < sizeof(sceneryBlocks) / sizeof(sceneryBlocks[0]); i++)
	{
		if (sceneryBlocks[i].isActive) // exlcude the dead 
		{
			spriteRect = { sceneryBlocks[i].x, sceneryBlocks[i].y, SPRITE_SIZE , SPRITE_SIZE };
			SDL_RenderCopyEx(renderer, sceneryBlocks[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	//shootable objects - crates
	for (int i = 0; i < sizeof(crateBlocks) / sizeof(crateBlocks[0]); i++)
	{
		if (crateBlocks[i].isActive) // dont render broken crates
		{
			spriteRect = { crateBlocks[i].x, crateBlocks[i].y, SPRITE_SIZE , SPRITE_SIZE };
			SDL_RenderCopyEx(renderer, crateBlocks[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	//render eggs 
	for (int i = 0; i < sizeof(eggs) / sizeof(eggs[0]); i++)
	{
		if (eggs[i].isActive) // dont render eaten eggs
		{
			spriteRect = { eggs[i].x, eggs[i].y, SPRITE_SIZE , SPRITE_SIZE };
			SDL_RenderCopyEx(renderer, eggs[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}


	//// enemies
	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		if (enemies[i].isAlive) // exclude the dead enemies
		{
			spriteRect = { enemies[i].x,  enemies[i].y, SPRITE_SIZE, SPRITE_SIZE };
			SDL_RenderCopyEx(renderer, enemies[i].spriteTexture, NULL, &spriteRect, enemies[i].angle, NULL, SDL_FLIP_NONE);
		}
	}
	//enemies 2
	for (int i = 0; i < sizeof(enemies2) / sizeof(enemies2[0]); i++)
	{
		if (enemies2[i].isAlive) // exclude the dead enemies
		{
			spriteRect = { enemies2[i].x,  enemies2[i].y, SPRITE_SIZE, SPRITE_SIZE };
			SDL_RenderCopyEx(renderer, enemies2[i].spriteTexture, NULL, &spriteRect, enemies2[i].angle, NULL, SDL_FLIP_NONE);
		}
	}
	//enemies 3
	for (int i = 0; i < sizeof(enemies3) / sizeof(enemies3[0]); i++)
	{
		if (enemies3[i].isAlive) // exclude the dead enemies
		{
			spriteRect = { enemies3[i].x,  enemies3[i].y, SPRITE_SIZE, SPRITE_SIZE };
			SDL_RenderCopyEx(renderer, enemies3[i].spriteTexture, NULL, &spriteRect, enemies3[i].angle, NULL, SDL_FLIP_NONE);
		}
	}



	// waypoints - IF WANT THEM SHOWN - commented out so player cannot see waypoints
	//for (int i = 0; i < sizeof(waypoints) / sizeof(waypoints[0]); i++)
	//{
	//	if (waypoints[i].isActive) // exclude those not active 
	//	{
	//		spriteRect = { waypoints[i].x,  waypoints[i].y, SPRITE_SIZE, SPRITE_SIZE };
	//		SDL_RenderCopy(renderer, waypoints[i].spriteTexture, NULL, &spriteRect);
	//	}
	//}


	// bullets Sprites
	if (bullet.isActive)
	{
		spriteRect = { bullet.x, bullet.y, bullet.size , bullet.size };
		SDL_RenderCopyEx(renderer, bullet.spriteTexture, NULL, &spriteRect, bullet.angle, NULL, SDL_FLIP_NONE);
	}

	// PC Sprite
	spriteRect = { pc.x, pc.y, SPRITE_SIZE , SPRITE_SIZE };
	SDL_RenderCopyEx(renderer, pc.spriteTexture, NULL, &spriteRect, pc.angle, NULL, SDL_FLIP_NONE);

	updateGUI();  //******** USER INTERFACE  ***********

	//Update the Screen
	SDL_RenderPresent(renderer);

}//-----


// =======================================================
void playerInputHandler()
{
	// Set up the input event handler
	while (SDL_PollEvent(&playerInputEvent) != 0)
	{
		switch (playerInputEvent.type)
		{
		case SDL_QUIT:
			gamePlaying = false;
			replay = false;
			break;

		case SDL_KEYDOWN:

			switch (playerInputEvent.key.keysym.sym)
			{
			case SDLK_w: pc.moveXY('u');	break;
			case SDLK_s: pc.moveXY('d');	break;
			case SDLK_a: pc.moveXY('l');	break;
			case SDLK_d: pc.moveXY('r');	break;


			case SDLK_SPACE: bullet.fire(pc.x, pc.y, pc.angle); Mix_PlayChannel(-1, shootSound, 0); break;
			case SDLK_q: quit = true; LevelCompleteScreen(); break; //Quits game
			case SDLK_r: restartScreen();  break;
				//case SDLK_p: currentLevel++; break;



			}
		}
	}
}


// =======================================================
void createGameSprites()
{
	// Background Image
	const char* spriteImageFile = "images/backgroundCow.png";
	SDL_Surface* spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Background Image Error  - SDL Error: %s\n", SDL_GetError());
	gameBackground = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Load and Create Sprite Textures
	// PC Sprite 
	spriteImageFile = "images/exterminator32.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	pc.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	// Bullet 
	spriteImageFile = "images/circle_8.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	bullet.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);


	// Waypoints
	spriteImageFile = "images/square_cross_grey.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(waypoints) / sizeof(waypoints[0]); i++)
	{
		waypoints[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	}

	// Enemies
	spriteImageFile = "images/pig32.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());

	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		enemies[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	//enemies 2
	spriteImageFile = "images/pig_grey.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());

	for (int i = 0; i < sizeof(enemies2) / sizeof(enemies2[0]); i++)
	{
		enemies2[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}


	//enemies 3
	spriteImageFile = "images/pig_purple.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());

	for (int i = 0; i < sizeof(enemies3) / sizeof(enemies3[0]); i++)
	{
		enemies3[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}


	// Set Terrain Blocks
	spriteImageFile = "images/grassPlatform32.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(terrainBlock) / sizeof(terrainBlock[0]); i++)
	{
		terrainBlock[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	//set background (non collidable)
	spriteImageFile = "images/mud32.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(backGroundBlocks) / sizeof(backGroundBlocks[0]); i++)
	{
		backGroundBlocks[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	//set scenery bushes - non collidable

	spriteImageFile = "images/Bush32.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(sceneryBlocks) / sizeof(sceneryBlocks[0]); i++)
	{
		sceneryBlocks[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	//set collidable crates
	spriteImageFile = "images/Crate32.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(crateBlocks) / sizeof(crateBlocks[0]); i++)
	{
		crateBlocks[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	//set eggs
	spriteImageFile = "images/egg32.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(eggs) / sizeof(eggs[0]); i++)
	{
		eggs[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	// Clear surface
	SDL_FreeSurface(spriteImage);

	//load audio
	music = Mix_LoadMUS("audio/BgMusic.wav");
	shootSound = Mix_LoadWAV("audio/bullet.wav");
	enemyDeathSound = Mix_LoadWAV("audio/enemyDeath.wav");
	destroySound = Mix_LoadWAV("audio/crateBreak.wav");
	playerHitSound = Mix_LoadWAV("audio/playerHit.wav");
	healSound = Mix_LoadWAV("audio/heal.wav");




}//-----


 // =======================================================
void LevelCompleteScreen()
{

	// is game quit
	if (quit == true)
	{
		const char* startImageFile = "images/quit.png"; //when game quit, this is what appears!
		SDL_Surface* startImage = IMG_Load(startImageFile);

		// merge image with surface
		SDL_BlitSurface(startImage, NULL, mainGameSurface, NULL);

		SDL_UpdateWindowSurface(gameWindow);
		SDL_Delay(1000);
		SDL_FreeSurface(startImage);
		replay = false;
		gamePlaying = false;
	}


	//is level lost
	if (pc.lives == 0)
	{
		// Load End Image
		const char* startImageFile = "images/end.png"; //when player dies, show died image
		SDL_Surface* startImage = IMG_Load(startImageFile);

		// merge image with surface
		SDL_BlitSurface(startImage, NULL, mainGameSurface, NULL);

		SDL_UpdateWindowSurface(gameWindow);
		SDL_Delay(2500);
		SDL_FreeSurface(startImage);


	}


	if (quit == false)
	{
		gamePlaying = true;
	}



}//-----


 // =======================================================
void welcomeScreen()
{
	// Load Title Image
	const char* startImageFile = "images/intro.png";
	SDL_Surface* startImage = IMG_Load(startImageFile);

	// merge image with surface
	SDL_BlitSurface(startImage, NULL, mainGameSurface, NULL);

	SDL_UpdateWindowSurface(gameWindow);
	SDL_Delay(1000);
	SDL_FreeSurface(startImage);
}//-----

void restartScreen()
{
	gamePlaying = false;
	goalReached = false;
	currentLevel = 1;

	//destroy alive enemies
	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		if (enemies[i].isAlive) // only those that are still alive
		{
			enemies[i].isAlive = false; //make them disappear
		}
	}
	for (int i = 0; i < sizeof(enemies2) / sizeof(enemies2[0]); i++)
	{
		if (enemies2[i].isAlive) // only those that are still alive
		{
			enemies2[i].isAlive = false; //make them disappear
		}
	}
	for (int i = 0; i < sizeof(enemies3) / sizeof(enemies3[0]); i++)
	{
		if (enemies3[i].isAlive) // only those that are still alive
		{
			enemies3[i].isAlive = false; //make them disappear
		}
	}

	//destroy crates
	for (int i = 0; i < sizeof(crateBlocks) / sizeof(crateBlocks[0]); i++)
	{
		if (crateBlocks[i].isActive) // only those that are still alive
		{
			crateBlocks[i].isActive = false; //make them disappear
		}
	}

	//destroy eggs
	for (int i = 0; i < sizeof(eggs) / sizeof(eggs[0]); i++)
	{
		if (eggs[i].isActive) // only those that are still alive
		{
			eggs[i].isActive = false; //make them disappear
		}
	}



	// Load Title Image
	const char* startImageFile = "images/intro.png";
	SDL_Surface* startImage = IMG_Load(startImageFile);

	// merge image with surface
	SDL_BlitSurface(startImage, NULL, mainGameSurface, NULL);

	SDL_UpdateWindowSurface(gameWindow);
	SDL_Delay(1000);
	SDL_FreeSurface(startImage);

}

//-----------------------------
void limitFrameRate(Uint32 startTick)
{
	Uint32 currentTick = SDL_GetTicks();

	if ((1000 / FPS) > currentTick - startTick) {
		SDL_Delay(1000 / FPS - (currentTick - startTick));
		frameTime = (float)1 / FPS;
	}
	else
	{
		frameTime = (float)(currentTick - startTick) / 1000;
	}
}//-----

 //-----------------------------
void startSDL()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) printf(" SDL init Error: %s\n", SDL_GetError());
	else
	{	//Create window
		gameWindow = SDL_CreateWindow("CI411 - SDL Game Cameron Crook", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (gameWindow == NULL) printf(" Window Error %s\n", SDL_GetError());
		//Get Game window surface
		mainGameSurface = SDL_GetWindowSurface(gameWindow);

		//Create a renderer for the window 
		renderer = SDL_CreateRenderer(gameWindow, -1, 0);

		//Set the draw color of renderer
		SDL_SetRenderDrawColor(renderer, 125, 125, 125, 0);

		// Initialise Fonts
		TTF_Init();
		font = TTF_OpenFont("fonts/pixeboy.ttf", 25);

		//add audio
		Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4069);
	}
}//-----

 //-----------------------------
void closeSDL()
{
	SDL_Delay(1000);

	//audio closing
	Mix_FreeMusic(music);
	Mix_FreeChunk(shootSound);
	Mix_FreeChunk(enemyDeathSound);
	Mix_FreeChunk(destroySound);
	Mix_FreeChunk(playerHitSound);
	Mix_CloseAudio();


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gameWindow);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();

}//-----

 // ================================