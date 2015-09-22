//general game class declaration and variables

#ifndef _GAME_H
#define _GAME_H
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Mmsystem.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "constants.h"
#include "graphics.h"
#include "textureManager.h"
#include "gameError.h"
#include "input.h"
#include "sound.h"
#include "Timer.h"
#include "camera.h"
#include "mesh.h"
#include "Player.h"
#include "Npc.h"
#include "Image.h"
#include "Object.h"
#include "Collisions.h"
//#include "console.h"

extern GridContents collisionGrid[GRID_HASH_SIZE];

class Game
{
protected:
	//game properties
	Graphics *graphics;		//graphics pointer
	Input *input;			//input pointer
	Sound *sound;			//sound engine pointer
//	Console *console;		//console pointer

	Player player;			//player interactive
	std::vector<Npc*> npc;	//ai characters
	std::vector<Object*> entity; //collideable non-ai objects
	std::vector<Object*> geometry; //collidable geometry/platforms
	std::vector<Image*> image; //non-colliable imagess

	//GridContents collisionGrid[GRID_HASH_SIZE];
	//CollGrid collisionGrid;

	MeshHandler meshHandler;		//geometry and mesh handler
	//make a final decision/rewrite for texture manager
	Object basicBlock; //testing-this will not be final

	HWND hwnd;				//window handler
	HRESULT hr;				//return type
	Timer *gameTimer;		//game timer
	float fFrameTime;		//time required for last frame
	float fFps;				//fps
	DWORD sleepTime;		//milliseconds to sleep between frames
	bool bPaused;			//true if game is paused
	bool bInitialized;		//true once game is initialized

	int nGameWidth;
	int nGameHeight;
	float fGameScale;
	bool bFullScreen;

	//camera variables
	float cameraX;
	float cameraY;
	float cameraZ;
	float targetX;
	float targetY;
	float targetZ;
	float upX;
	float upY;
	float upZ;
	float ry;
	float rxz;
	int incA;

	//lighting
	LightProperties lighting;
	UINT numLights;

	//testing stuff
	float lAngle;

	//temp cam variables	
	int incX;
	int incY;
	int incZ;
	float theta;
	float gamma;
	float length;

	void clearGridObject(ObjectPtrList *o);
	void clearGridNpc(NpcPtrList *n);
	void clearGridContents(GridContents *g);

public:

	std::string  command;           //command from console

	Game();					//constructor
	virtual ~Game();		//destructor
	LRESULT messageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);  //windows message handler
	virtual void initialize(HWND hwnd, UINT w, UINT h, bool f);		//initialize the game
	virtual void run(HWND);					//called repeatedly by windows message loop
	virtual void releaseAll();				//release video memory so graphics device can reset
	virtual void resetAll();				//recreate all surfaces, reset all entities
	virtual void deleteAll();				//clear all reserved memory
	virtual void renderGame();				//renders the game

	Graphics *getGraphics() { return graphics; }//return pointer to graphics
	Input *getInput() { return input; }			//return pointer to input
	Sound *getSound() { return sound; }			//return pointer to sound engine

	//exits game
	void exitGame()
	{
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}

	virtual void update();		//update game items
	virtual void ai();			//perform ai calculations
	virtual void render();		//handle the drawing
	virtual void geometryCollide();
	virtual void objectCollide();
	virtual void updateCollisionGrid();
	virtual void emptyCollisionGrid();

	//console command processor
//	virtual void consoleCommand();

	virtual void loadConfig();
	virtual void saveConfig();
	int convertString(std::string s);

	//level loading
	virtual bool loadLevel(int level);
};
#endif