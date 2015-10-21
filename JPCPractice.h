#ifndef _JPCPractice_H
#define _JPCPractice_H
#define WIN32_LEAN_AND_MEAN

#include "Game.h"
#include <locale>

struct Question
{
	int difficulty[6];
	std::wstring questionText;
	std::wstring answerText;

	Question()
	{
		difficulty[0] = -1;
		questionText = L"The question goes here....";
		answerText = L"....and the answer (with other info) goes here";
	}
};

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

	//round one lists
	std::vector<Question*> kanji;
	std::vector<Question*> kotowaza;
	std::vector<Question*> kasanekotoba;
	std::vector<Question*> animalSounds;
	std::vector<Question*> culture;

	//round two lists
	std::vector<Question*> placeNames;
	std::vector<Question*> yojukugo;
	std::vector<Question*> idioms;
	std::vector<Question*> katakana;
	std::vector<Question*> grammar;

	//arrays for game questions, rounds 1 and 2
	Question round1Questions[25];
	Question round2Questions[25];

	void randomizeCategory(std::vector<Question*> *category, int offset, int round);
	std::wstring fromUTF8(const char* str);

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

	void loadQuestionFiles();
	void assignQuestions(int round);
};

#endif