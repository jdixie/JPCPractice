#ifndef _JPCPractice_H
#define _JPCPractice_H
#define WIN32_LEAN_AND_MEAN

#include "Game.h"

class JPCPractice : public Game
{
private:
	int numberChosen;
	bool chosenQuestion[25];
	int currentLevel = 0;

	void initializeChosenQuestions() {
		numberChosen = 0;
		for (int i = 0; i < 25; i++)
			chosenQuestion[i] = false;
	}
	std::vector<TextImageInfo*> questionTextImages; //question text screens for the actual question
	std::vector<TextImageInfo*> answerTextImages; //answer text screens for each question
	bool questionMode = false;
	bool answerMode = false;
	bool animating = false;
	bool questionWaiting = false;
	int questionCountdown = 30;
	int questionIndex = -1;
	float questionScreenOriginalX;
	float questionScreenOriginalY;
	float questionScreenOriginalZ;
	float xIncr;
	float yIncr;
	float zIncr;
	int passes;
	float animationDivisions = 30;

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