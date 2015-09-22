#ifndef _JPCPractice_H
#define _JPCPractice_H
#define WIN32_LEAN_AND_MEAN

#include "Game.h"

class JPCPractice : public Game
{
private:
	Vertex vtTest[8];
	UINT indices[36];

public:
	JPCPractice();
	~JPCPractice();

	void initialize(HWND hw);
	void render();
	void ai();
	void update();

	void geometryCollide();
	void objectCollide();

	void loadConfig();
	void saveConfig();
	bool loadLevel(int level);
};

#endif