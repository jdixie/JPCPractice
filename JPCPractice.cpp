#include "JPCPractice.h"

JPCPractice::JPCPractice()
{
	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 0.0f;
	ry = 0;
	rxz = 0;
	incX = 0;
	incY = 0;
	theta = 0;
	gamma = 0;
	length = -1000;
}

JPCPractice::~JPCPractice()
{

}

void JPCPractice::initialize(HWND hw)
{
	loadConfig();

	Game::initialize(hw, nGameWidth, nGameHeight, bFullScreen);

	ShowCursor(true);

	if (bFullScreen)
	{
		SetWindowLong(hwnd, GWL_STYLE, WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
	}
	else
	{
		SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, nGameWidth, nGameHeight,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

		// Adjust window size so client area is GAME_WIDTH x GAME_HEIGHT
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);   // get size of client area of window
		MoveWindow(hwnd,
			0,                                           // Left
			0,                                           // Top
			nGameWidth, //+(nGameWidth-clientRect.right),    // Right
			nGameHeight, //+(nGameHeight-clientRect.bottom), // Bottom
			TRUE);                                       // Repaint the window
	}

	switch (nGameHeight)
	{
	case 1080:
		fGameScale = 1;
		break;
	case 1050:
		fGameScale = .97;
		break;
	case 1024:
		fGameScale = .94;
		break;
	case 960:
		fGameScale = .88;
		break;
	case 900:
		fGameScale = .83;
		break;
	case 864:
		fGameScale = .8;
		break;
	case 800:
		fGameScale = .74;
		break;
	case 768: default:
		fGameScale = .71;
		break;
	}

	player.initialize(input, graphics, 0, 0, 1);

	//set up play screens: vertically 0-4 are column 1 100-500, 5-9 are column 2 100-500, etc.
	//25-29 are the category screens across the top
	//center of each play screen, since looking down x axis, all x will be the same

	//mouse pointer
	pointer.initialize(graphics, 0, 0, 1);
	pointer.getImageInfo()->mesh = new MeshData;
	meshHandler.createRect(pointerSize, pointerSize, pointer.getImageInfo());
	pointer.getImageInfo()->mesh->textureMapSRV.push_back(textureManager.loadTexture(L"mesh\\arrow.png"));
	for (int i = 0; i < 4; i++)
	{
		pointer.getImageInfo()->mesh->vertices.at(i).position.y = pointer.getImageInfo()->mesh->vertices.at(i).position.z;
		pointer.getImageInfo()->mesh->vertices.at(i).position.z = 0;
	}
	pointer.setVertices();
	pointer.getImageInfo()->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
	pointer.getImageInfo()->mesh->materials.at(0).Mat.Emissive = DirectX::XMFLOAT4(1, 1, 1, 1);
	pointer.rotate(90, 0, 0, 1);
	pointer.setZ(1);

	//load and ready bluescreens
	float yc = 225;
	float xc = -500;
	for (int i = 0; i < 30; i++)
	{
		Object *screen = new Object;
		screen->initialize(graphics, 0, 0, 1);
		screen->getImageInfo()->mesh = new MeshData;
		meshHandler.createRect(245, 145, screen->getImageInfo());
		//screen->getImageInfo()->mesh->textureMapSRV.push_back(graphics->createTextureSRV(L"mesh\\darkblue.jpg"));
		screen->getImageInfo()->mesh->textureMapSRV.push_back(textureManager.loadTexture(L"mesh\\darkblue.jpg"));
		screen->setVertices();
		entity.push_back(screen);
		entity.at(i)->getImageInfo()->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
		entity.at(i)->rotate(90, 1, 0, 0);
		entity.at(i)->setZ(1050);

		if (i < 25)
		{
			if (i % 5 == 0 & i != 0)
			{
				yc = 225;
				xc += 250;
			}

			entity.at(i)->setY(yc);
			yc -= 150;
			entity.at(i)->setX(xc);

			if (i == 24) //set-up for category play screens
			{
				yc = 375;
				xc = -500;
			}
		}
		else
		{
			entity.at(i)->setY(yc);
			entity.at(i)->setX(xc);
			xc += 250;
		}
	}

	loadQuestionFiles();
	loadLevel(1);

	graphics->camera.setCamType(CamType::FPS);
	graphics->camera.initialize();
	//graphics->camera.setPosition(0, 0, 0, 0, 90);
	graphics->camera.setPosition(0, 0, 0, 0, 0, 1, 0, 1, 0);
	rxz = 90;

	initializeChosenQuestions();
}

void JPCPractice::update()
{
	Vector3 rayPos, rayDir;
	int mouseX, mouseY;
	if (bFullScreen)
	{
		mouseX = input->getMouseX() * fGameScale;
		mouseY = input->getMouseY() * fGameScale;
	}
	else
	{
		mouseX = input->getMouseX();
		mouseY = input->getMouseY();
	}
	graphics->getPickRay(mouseX, mouseY, &rayPos, &rayDir);
	pointer.setX(rayDir.x + pointerSize / 2);
	pointer.setY(-rayDir.y - pointerSize / 2);

	if (gameState == GameState::ANIMATING)
	{
		if (passes <= animationDivisions)
		{
			entity.at(questionIndex)->setX(entity.at(questionIndex)->getX() + xIncr);
			entity.at(questionIndex)->setY(entity.at(questionIndex)->getY() + yIncr);
			entity.at(questionIndex)->setZ(entity.at(questionIndex)->getZ() + zIncr);
			textImages.at(questionIndex)->x = textImages.at(questionIndex)->x + xIncr;
			textImages.at(questionIndex)->y = textImages.at(questionIndex)->y + yIncr;
			textImages.at(questionIndex)->z = textImages.at(questionIndex)->z + zIncr;
			if (entity.at(questionIndex)->getZ() < 170)
			{
				entity.at(questionIndex)->setX(0);
				entity.at(questionIndex)->setY(0);
				entity.at(questionIndex)->setZ(170);
				textImages.at(questionIndex)->x = 0;
				textImages.at(questionIndex)->y = 0;
				textImages.at(questionIndex)->z = 169;
			}

			passes++;
		}
		else
		{
			gameState = GameState::QUESTIONWAITING;
		}
	}
	else if (gameState == GameState::QUESTIONWAITING)
	{
		questionCountdown--;
		if (questionCountdown < 0)
		{
			gameState = GameState::QUESTIONMODE;
			questionCountdown = 20;
			chosenQuestion[questionIndex] = true;
			numberChosen++;
		}
	}
	else if (gameState == GameState::QUESTIONMODE)
	{
		if (input->getMouseLButton())
		{
			input->setMouseLButton(false);
			gameState = GameState::ANSWERMODE;
		}
	}
	else if (gameState == GameState::ANSWERMODE)
	{
		if (input->getMouseRButton())
		{
			gameState = GameState::QUESTIONMODE;
		}
		if (input->getMouseLButton())
		{
			input->setMouseLButton(false);
			entity.at(questionIndex)->setX(questionScreenOriginalX);
			entity.at(questionIndex)->setY(questionScreenOriginalY);
			entity.at(questionIndex)->setZ(questionScreenOriginalZ);
			textImages.at(questionIndex)->x = questionScreenOriginalX;
			textImages.at(questionIndex)->y = questionScreenOriginalY;
			textImages.at(questionIndex)->z = questionScreenOriginalZ - 1;
			entity.at(questionIndex)->getImageInfo()->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 0, 0, 1);
			questionIndex = -1;
			gameState = GameState::CHOOSING;
		}
	}
	else
	{
		if (numberChosen == 25 && currentLevel == 1)
		{
			initializeChosenQuestions();
			loadLevel(2);
		}

		if (input->getMouseLButton())
		{
			input->setMouseLButton(false);
			//compute ray for clicked spot  in view space
			Vector3 dirFrac;
			//Vector3 rayPos, rayDir, dirFrac;
			/*int mouseX, mouseY;
			if (bFullScreen)
			{
				mouseX = input->getMouseX() * fGameScale;
				mouseY = input->getMouseY() * fGameScale;
			}
			else
			{
				mouseX = input->getMouseX();
				mouseY = input->getMouseY();
			}
			graphics->getPickRay(mouseX, mouseY, &rayPos, &rayDir);*/

			for (int i = 0; i < 25; i++)
			{
				dirFrac.x = 1.0f / rayDir.x;
				dirFrac.y = -1.0f / rayDir.y;
				dirFrac.z = 1.0f / rayDir.z;

				float t1 = (entity.at(i)->getX() - 122.5 - rayPos.x)*dirFrac.x;
				float t2 = (entity.at(i)->getX() + 122.5 - rayPos.x)*dirFrac.x;
				float t3 = (entity.at(i)->getY() - 72.5 - rayPos.y)*dirFrac.y;
				float t4 = (entity.at(i)->getY() + 72.5 - rayPos.y)*dirFrac.y;
				float t5 = (entity.at(i)->getZ() - rayPos.z)*dirFrac.z;
				//float t6 = (entity.at(i)->getZ() - rayPos.z)*dirFrac.z;

				float tmin = max(max(min(t1, t2), min(t3, t4)), t5);
				float tmax = min(min(max(t1, t2), max(t3, t4)), t5);

				//if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
				if (tmax < 0)
				{
					continue;
				}
				//if tmin > tmax, ray doesn't intersect AABB
				else if (tmin > tmax)
				{
					continue;
				}
				//entity.at(i) was picked
				else
				{
					//entity.at(i)->getImageInfo()->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 0, 0, 1);
					//chosenQuestion[i] = true;
					//numberChosen++;

					//if (chosenQuestion[i] == false)
					//{
						questionIndex = i;
						questionScreenOriginalX = entity.at(i)->getX();
						questionScreenOriginalY = entity.at(i)->getY();
						questionScreenOriginalZ = entity.at(i)->getZ();
						xIncr = (0 - questionScreenOriginalX) / animationDivisions;
						yIncr = (0 - questionScreenOriginalY) / animationDivisions;
						zIncr = (-880 / animationDivisions);
						passes = 0;

						gameState = GameState::ANIMATING;
						break;
					//}
				}
			}
		}
	}


	//update collisionGrid - clear entries
	//updateCollisionGrid();

	//update player, npc's, objects, and images to set their temporary movement variables
	//player.update(fFrameTime, fGameScale);



	//check fo collisions between player and geometry, npc and geometry, player and npc, objects and everything else
	//loop it to run 3-ish times to check for new collisions after adjustments
	//debug - call only on movement
	//if (player.getVelocity().x != 0 || player.getVelocity().y != 0 || player.getVelocity().z != 0)
	//	geometryCollide();

	//	player.finalizeMovement();

	//update lighting
	/*lAngle += .05;
	if (lAngle >= 360)
		lAngle = 0;
	float sa = 500 * sin(lAngle);
	float ca = 500 * cos(lAngle);
	lighting.Lights[0].Position = DirectX::XMFLOAT4(sa, 500, ca, 1);
	lighting.Lights[1].Position = DirectX::XMFLOAT4(-2500 + sa, 500, -2500 + ca, 1);
	lighting.Lights[2].Position = DirectX::XMFLOAT4(-2500 + sa, 500, 2500 + ca, 1);
	lighting.Lights[3].Position = DirectX::XMFLOAT4(2500 + sa, 500, -2500 + ca, 1);
	lighting.Lights[4].Position = DirectX::XMFLOAT4(2500 + sa, 500, 2500 + ca, 1);*/
}

void JPCPractice::geometryCollide()
{
	Game::geometryCollide();
}

void JPCPractice::objectCollide()
{
	Game::objectCollide();
}

void JPCPractice::ai()
{
	Game::ai();
}

void JPCPractice::render()
{
	//	graphics->setViewMatrix(nGameWidth/2, nGameHeight/2, -1200);
	//camera.setPosition(cameraX, cameraY, cameraZ, targetX, targetY, targetZ, upX, upY, upZ);
	//std::wstringstream a;
	//a << "rxz @ JPCPractice: " << rxz << "\n";
	//OutputDebugString(a.str().c_str());
	//graphics->camera.setPosition(cameraX, cameraY, cameraZ, ry, rxz);
	graphics->setViewMatrix();
	
	//player.draw();
	for (int i = 0; i < geometry.size(); i++)
		geometry[i]->draw();
	for (int i = 0; i < entity.size(); i++)
		entity[i]->draw();
	for (int i = 0; i < image.size(); i++)
		image[i]->draw();
	for (int i = 0; i < npc.size(); i++)
		npc[i]->draw();
	//graphics->renderText(L"Test テスト");
	//graphics->renderText(L"Over or replace?");
	for (int i = 0; i < textImages.size(); i++)
	{
		if ((i < 25 && chosenQuestion[i] == false) || (i >= 25))
			graphics->drawTextRect(textImages.at(i));
	}
	if (gameState != GameState::QUESTIONMODE && gameState != GameState::ANSWERMODE)
	{
		graphics->setBlendStateTransparent(pointer.getBlendFactors());
		pointer.draw();
		graphics->setBlendStateNotTransparent();
	}
	if (gameState == GameState::QUESTIONMODE)
		graphics->drawTextRect(questionTextImages.at(questionIndex));
	if (gameState == GameState::ANSWERMODE)
		graphics->drawTextRect(answerTextImages.at(questionIndex));
}

void JPCPractice::loadConfig()
{
	std::string s;
	std::string v;
	std::ifstream configFile;
	configFile.open("JCP.cfg");

	if (configFile.is_open())
	{
		while (configFile >> s)
		{
			configFile >> v;
			if (s == "bFullScreen")
			{
				if (v == "true")
					bFullScreen = true;
				else
					bFullScreen = false;
			}
			else if (s == "width")
			{
				std::stringstream convert(v);
				if (!(convert >> nGameWidth))
					nGameWidth = 1024;
			}
			else if (s == "height")
			{
				std::stringstream convert(v);
				if (!(convert >> nGameHeight))
					nGameHeight = 768;
			}
		}
	}
	else
	{
		bFullScreen = false;
		nGameWidth = 1024;
		nGameHeight = 768;
		saveConfig();
	}

	configFile.close();
}

void JPCPractice::saveConfig()
{
	std::ofstream configFile;
	configFile.open("Auramancer.cfg");

	configFile << "bFullScreen " << bFullScreen << std::endl;
	configFile << "width " << nGameWidth << std::endl;
	configFile << "height " << nGameHeight << std::endl;

	configFile.close();
}

bool JPCPractice::loadLevel(int level)
{
	std::string s;
	std::string v;
	std::ifstream levelFile;
	int tempLevel;

	//play screen reminder: vertically 0-4 are column 1 100-500, 5-9 are column 2 100-500, etc.
	//25-29 are the category screens across the top
	//center of each play screen, since looking down x axis, all x will be the same

	//load questions and text for screens
	float yc = 225;
	float xc = -500;
	switch (level)
	{
	case 2:
		textImages.clear();
		questionTextImages.clear();
		answerTextImages.clear();
		for (int i = 0; i < 25; i++)
		{
			entity.at(i)->getImageInfo()->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
			TextImageInfo *temp = new TextImageInfo;
			temp->mesh = new MeshData;
			meshHandler.createRect(245, 145, temp);
			temp->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
			temp->mesh->materials.at(0).Mat.Emissive = DirectX::XMFLOAT4(1, 1, 1, 1);

			int value = ((i % 5) * 100 + 100) * 2;
			temp->text = std::to_wstring(value);

			textImages.push_back(temp);
			graphics->InitD2DRectTexture(textImages.at(i), 256);
			textImages.at(i)->z = 1049;
			textImages.at(i)->rotation.makeRotate(-90, 1, 0, 0);

			if (i % 5 == 0 & i != 0)
			{
				yc = 225;
				xc += 250;
			}

			textImages.at(i)->y = yc;
			yc -= 150;
			textImages.at(i)->x = xc;
		}

		yc = 375;
		xc = -500;

		for (int i = 25; i < 30; i++)
		{
			TextImageInfo *temp = new TextImageInfo;
			temp->mesh = new MeshData;
			meshHandler.createRect(245, 145, temp);
			temp->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
			temp->mesh->materials.at(0).Mat.Emissive = DirectX::XMFLOAT4(1, 1, 1, 1);

			switch (i)
			{
			case 25:
				temp->text = L"Place names";
				break;
			case 26:
				temp->text = L"よじゅくご";
				break;
			case 27:
				temp->text = L"Idioms";
				break;
			case 28:
				temp->text = L"かたかな";
				break;
			case 29:
				temp->text = L"Language Tasks";
				break;
			default:
				temp->text = L"Category";
				break;
			}

			textImages.push_back(temp);
			graphics->InitD2DRectTexture(textImages.at(i), 144);
			textImages.at(i)->z = 1049;
			textImages.at(i)->rotation.makeRotate(-90, 1, 0, 0);

			textImages.at(i)->y = yc;
			textImages.at(i)->x = xc;
			xc += 250;
		}

		assignQuestions(2);
		//load question screens and answers
		for (int i = 0; i < 25; i++)
		{
			TextImageInfo *temp1 = new TextImageInfo;
			temp1->mesh = new MeshData;
			meshHandler.createRect(245, 145, temp1);
			temp1->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
			temp1->mesh->materials.at(0).Mat.Emissive = DirectX::XMFLOAT4(1, 1, 1, 1);

			TextImageInfo *temp2 = new TextImageInfo;
			temp2->mesh = new MeshData;
			meshHandler.createRect(245, 145, temp2);
			temp2->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
			temp2->mesh->materials.at(0).Mat.Emissive = DirectX::XMFLOAT4(1, 1, 1, 1);

			temp1->text = L"The question goes here....";
			temp2->text = L"....and the answer (with other info) goes here";

			questionTextImages.push_back(temp1);
			if (questionTextImages.at(i)->text.length() > 256)
				graphics->InitD2DRectTexture(questionTextImages.at(i), 48);
			else
				graphics->InitD2DRectTexture(questionTextImages.at(i), 64);
			questionTextImages.at(i)->z = 169;
			questionTextImages.at(i)->rotation.makeRotate(-90, 1, 0, 0);

			answerTextImages.push_back(temp2);
			if (answerTextImages.at(i)->text.length() > 256)
				graphics->InitD2DRectTexture(answerTextImages.at(i), 48);
			else
				graphics->InitD2DRectTexture(answerTextImages.at(i), 64);
			answerTextImages.at(i)->z = 169;
			answerTextImages.at(i)->rotation.makeRotate(-90, 1, 0, 0);
		}

		currentLevel = 2;
		break;
	case 1:
	default:
		for (int i = 0; i < 25; i++)
		{
			entity.at(i)->getImageInfo()->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
			TextImageInfo *temp = new TextImageInfo;
			temp->mesh = new MeshData;
			meshHandler.createRect(245, 145, temp);
			temp->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
			temp->mesh->materials.at(0).Mat.Emissive = DirectX::XMFLOAT4(1, 1, 1, 1);

			int value = (i % 5) * 100 + 100;
			temp->text = std::to_wstring(value);

			textImages.push_back(temp);
			graphics->InitD2DRectTexture(textImages.at(i), 256);
			textImages.at(i)->z = 1049;
			textImages.at(i)->rotation.makeRotate(-90, 1, 0, 0);

			if (i % 5 == 0 & i != 0)
			{
				yc = 225;
				xc += 250;
			}

			textImages.at(i)->y = yc;
			yc -= 150;
			textImages.at(i)->x = xc;
		}

		yc = 375;
		xc = -500;

		for (int i = 25; i < 30; i++)
		{
			TextImageInfo *temp = new TextImageInfo;
			temp->mesh = new MeshData;
			meshHandler.createRect(245, 145, temp);
			temp->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
			temp->mesh->materials.at(0).Mat.Emissive = DirectX::XMFLOAT4(1, 1, 1, 1);

			switch (i)
			{
			case 25:
				temp->text = L"かんじ";
				break;
			case 26:
				temp->text = L"ことわざ";
				break;
			case 27:
				temp->text = L"かさねことば";
				break;
			case 28:
				temp->text = L"Animal Sounds";
				break;
			case 29:
				temp->text = L"Culture";
				break;
			default:
				temp->text = L"Category";
				break;
			}		

			textImages.push_back(temp);
			graphics->InitD2DRectTexture(textImages.at(i), 144);
			textImages.at(i)->z = 1049;
			textImages.at(i)->rotation.makeRotate(-90, 1, 0, 0);

			textImages.at(i)->y = yc;
			textImages.at(i)->x = xc;
			xc += 250;
		}

		assignQuestions(1);
		//load question screens and answers
		for (int i = 0; i < 25; i++)
		{
			TextImageInfo *temp1 = new TextImageInfo;
			temp1->mesh = new MeshData;
			meshHandler.createRect(245, 145, temp1);
			temp1->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
			temp1->mesh->materials.at(0).Mat.Emissive = DirectX::XMFLOAT4(1, 1, 1, 1);

			TextImageInfo *temp2 = new TextImageInfo;
			temp2->mesh = new MeshData;
			meshHandler.createRect(245, 145, temp2);
			temp2->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
			temp2->mesh->materials.at(0).Mat.Emissive = DirectX::XMFLOAT4(1, 1, 1, 1);
			
			temp1->text = round1Questions[i].questionText;
			temp2->text = round1Questions[i].answerText;

			questionTextImages.push_back(temp1);
			if (questionTextImages.at(i)->text.length() > 256)
				graphics->InitD2DRectTexture(questionTextImages.at(i), 48);
			else
				graphics->InitD2DRectTexture(questionTextImages.at(i), 64);
			questionTextImages.at(i)->z = 169;
			questionTextImages.at(i)->rotation.makeRotate(-90, 1, 0, 0);

			answerTextImages.push_back(temp2);
			if (answerTextImages.at(i)->text.length() > 256)
				graphics->InitD2DRectTexture(answerTextImages.at(i), 48);
			else
				graphics->InitD2DRectTexture(answerTextImages.at(i), 64);
			answerTextImages.at(i)->z = 169;
			answerTextImages.at(i)->rotation.makeRotate(-90, 1, 0, 0);
		}

		currentLevel = 1;
		break;
	}

	return true;
}

void JPCPractice::loadQuestionFiles()
{
	//grab a list of all file in the questions directory
	WCHAR wstrDir[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, wstrDir);
	std::wstring searchDir(wstrDir);
	searchDir.append(L"\\questions\\*.*");
	std::vector<std::wstring> files;
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFileW(searchDir.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				files.push_back(fd.cFileName);
			}
		} while (FindNextFileW(hFind, &fd));
		::FindClose(hFind);
	}
	
	//fill the appropriate category question arrays
	std::wstring qdir(wstrDir);
	qdir.append(L"\\questions\\");
	SetCurrentDirectory(qdir.c_str());
	for (int i = 0; i < files.size(); i++)
	{
		Question *temp;
		int category = -1;
		bool done = false;
		bool skip = false;
		std::string s;
		std::string v;
		std::wstring us;
		std::wstring uv;
		std::ifstream questionFile;
		questionFile.open(files.at(i), std::ios::binary);

		temp = new Question;
		while (questionFile >> s)
		{
			us = fromUTF8(s.c_str());
			if (temp == NULL)
				temp = new Question;
			if (us.at(0) == 0xEF && us.at(1) == 0xBB && us.at(2) == 0xBF)
			{
				us = us.substr(3, us.length() - 1);
			}
			if (us.at(0) == 65279)
				us = us.substr(1, us.length() - 1);
			if (us == L"<category>")
			{
				questionFile >> v;
				uv = fromUTF8(v.c_str());
				if (uv == L"</category>")
				{
					delete temp;
					continue;
				}
				else if (uv == L"かんじ" || uv == L"kanji" || uv == L"Kanji")
					category = 0;
				else if (uv == L"ことわざ" || uv == L"kotowaza" || uv == L"proverbs" || uv == L"Kotowaza" || uv == L"Proverbs")
					category = 1;
				else if (uv == L"かさねことば" || uv == L"kasanekotoba" || uv == L"onomatopoeia" || uv == L"Kasanekotoba" || uv == L"Onomatopoeia")
					category = 2;
				else if (uv == L"AnimalSounds" || uv == L"animalsounds")
					category = 3;
				else if (uv == L"Culture" || uv == L"culture")
					category = 4;
				else if (uv == L"PlaceNames" || uv == L"placenames")
					category = 5;
				else if (uv == L"よじゅくご" || uv == L"yojukugo" || uv == L"Yojukugo")
					category = 6;
				else if (uv == L"Idioms" || uv == L"idioms")
					category = 7;
				else if (uv == L"かたかな" || uv == L"katakana" || uv == L"Katakana")
					category = 8;
				else if (uv == L"LanguageTasks" || uv == L"languagetasks" || uv == L"Grammar" || uv == L"grammar")
					category = 9;

				questionFile >> v;
				uv = fromUTF8(v.c_str());
				if (uv != L"</category>")
				{
					skip = true;
					continue;
				}
			}
				
			else if (us == L"<difficulty>")
			{
				questionFile >> v;
				uv = fromUTF8(v.c_str());
				if (uv.length() > 1)
				{
					int pos = 0;
					for (int j = 0; j < uv.length(); j += 2)
					{
						if (j >= 12)
							break;
						std::wstringstream convert(uv.at(j));
						if (!(convert >> temp->difficulty[j/2]))
						{
							if (j == 0)
								temp->difficulty[j / 2] = -1;
							else
								temp->difficulty[j / 2] = 0;
						}
					}
				}
				else
				{
					std::wstringstream convert(uv);
					if (!(convert >> temp->difficulty[0]))
					{
						//do nothing?
					}
				}
				questionFile >> v;
				uv = fromUTF8(v.c_str());
				if (uv != L"</difficulty")
				{
					skip = true;
				}
			}

			else if (us == L"<question>")
			{
				questionFile >> v;
				uv = fromUTF8(v.c_str());
				temp->questionText = L"";
				while (uv != L"</question>")
				{
					if (uv == L"<\\n>")
					{
						temp->questionText.append(L"\n");
					}
					else
					{
						temp->questionText.append(uv);
						temp->questionText = temp->questionText + L" ";
					}
					questionFile >> v;
					uv = fromUTF8(v.c_str());
				}
				if (questionFile.eof())
				{
					skip = true;
				}
			}

			else if (us == L"<answer>")
			{
				questionFile >> v;
				uv = fromUTF8(v.c_str());
				temp->answerText = L"";
				while (uv != L"</answer>")
				{
					if (uv == L"<\\n>")
					{
						temp->answerText.append(L"\n");
					}
					else
					{
						temp->answerText.append(uv);
						temp->answerText = temp->answerText + L" ";
					}
					questionFile >> v;
					uv = fromUTF8(v.c_str());
				}
				if (uv != L"</answer>" && questionFile.eof())
				{
					skip = true;
				}
				done = true;
			}
			
			if (skip)
			{
				delete temp;
				skip = false;
				continue;
			}

			if (done)
			{
				switch (category)
				{
				case 0:
					kanji.push_back(temp);
					break;
				case 1:
					kotowaza.push_back(temp);
					break;
				case 2:
					kasanekotoba.push_back(temp);
					break;
				case 3:
					animalSounds.push_back(temp);
					break;
				case 4:
					culture.push_back(temp);
					break;
				case 5:
					placeNames.push_back(temp);
					break;
				case 6:
					yojukugo.push_back(temp);
					break;
				case 7:
					idioms.push_back(temp);
					break;
				case 8:
					katakana.push_back(temp);
					break;
				case 9:
					grammar.push_back(temp);
					break;
				default:
					delete temp;
				}
				temp = new Question;
				done = false;
			}
			
		}//end while
	}

	SetCurrentDirectory(wstrDir);
}

void JPCPractice::assignQuestions(int round)
{
	switch (round)
	{
	case 2:
		randomizeCategory(&placeNames, 0, round);
		randomizeCategory(&yojukugo, 5, round);
		randomizeCategory(&idioms, 10, round);
		randomizeCategory(&katakana, 15, round);
		randomizeCategory(&grammar, 20, round);
		break;
	case 1:
	default:
		//randomize the questions within their categories, provided no difficulties assigned
		randomizeCategory(&kanji, 0, 1);
		randomizeCategory(&kotowaza, 5, 1);
		randomizeCategory(&kasanekotoba, 10, 1);
		randomizeCategory(&animalSounds, 15, 1);
		randomizeCategory(&culture, 20, 1);
	}
}

void JPCPractice::randomizeCategory(std::vector<Question*> *category, int offset, int round)
{
	std::vector<int> diff1, diff2, diff3, diff4, diff5;
	int chosen;
	srand(time(NULL));

	for (int i = 0; i < category->size(); i++)
	{
		if (category->at(i)->difficulty[0] == -1)
		{
			bool good;
			do
			{
				good = true;
				chosen = rand() % 5 + 1;
				switch (chosen)
				{
				case 1:
					if (diff1.size() != 0 && (diff2.size() == 0 || diff3.size() == 0 || diff4.size() == 0 || diff5.size() == 0))
					{
						chosen = 2;
						good = false;
					}
					else
					{
						diff1.emplace_back(i);
						good = true;
						break;
					}
				case 2:
					if (diff2.size() != 0 && (diff1.size() == 0 || diff3.size() == 0 || diff4.size() == 0 || diff5.size() == 0))
					{
						chosen = 3;
						good = false;
					}
					else
					{
						diff2.emplace_back(i);
						good = true;
						break;
					}
				case 3:
					if (diff3.size() != 0 && (diff2.size() == 0 || diff1.size() == 0 || diff4.size() == 0 || diff5.size() == 0))
					{
						chosen = 4;
						good = false;
					}
					else
					{
						diff3.emplace_back(i);
						good = true;
						break;
					}
				case 4:
					if (diff4.size() != 0 && (diff2.size() == 0 || diff3.size() == 0 || diff1.size() == 0 || diff5.size() == 0))
					{
						chosen = 5;
						good = false;
					}
					else
					{
						diff4.emplace_back(i);
						good = true;
						break;
					}
				case 5:
					if (diff5.size() != 0 && (diff2.size() == 0 || diff3.size() == 0 || diff4.size() == 0 || diff1.size() == 0))
						good = false;
					else
					{
						diff5.emplace_back(i);
						good = true;
						break;
					}
				default:
					break;
				}
			} while (!good);
		}
	}

	//randomly choose one for each difficulty and assign it
	chosen = rand() % diff1.size();
	round1Questions[offset] = *category->at(diff1.at(chosen));
	chosen = rand() % diff2.size();
	round1Questions[offset + 1] = *category->at(diff2.at(chosen));
	chosen = rand() % diff3.size();
	round1Questions[offset + 2] = *category->at(diff3.at(chosen));
	chosen = rand() % diff4.size();
	round1Questions[offset + 3] = *category->at(diff4.at(chosen));
	chosen = rand() % diff5.size();
	round1Questions[offset + 4] = *category->at(diff5.at(chosen));

	//clear the diff vectors
	diff1.clear();
	diff2.clear();
	diff3.clear();
	diff4.clear();
	diff5.clear();
}

std::wstring JPCPractice::fromUTF8(const char* str)
{
	const unsigned char* s = reinterpret_cast<const unsigned char*>(str);

	static const wchar_t badchar = '?';

	std::wstring ret;

	unsigned i = 0;
	while (s[i])
	{
		try
		{
			if (s[i] < 0x80)         // 00-7F: 1 byte codepoint
			{
				ret += s[i];
				++i;
			}
			else if (s[i] < 0xC0)    // 80-BF: invalid for midstream
				throw 0;
			else if (s[i] < 0xE0)    // C0-DF: 2 byte codepoint
			{
				if ((s[i + 1] & 0xC0) != 0x80)		throw 1;

				ret += ((s[i] & 0x1F) << 6) |
					((s[i + 1] & 0x3F));
				i += 2;
			}
			else if (s[i] < 0xF0)    // E0-EF: 3 byte codepoint
			{
				if ((s[i + 1] & 0xC0) != 0x80)		throw 1;
				if ((s[i + 2] & 0xC0) != 0x80)		throw 2;

				wchar_t ch =
					((s[i] & 0x0F) << 12) |
					((s[i + 1] & 0x3F) << 6) |
					((s[i + 2] & 0x3F));
				i += 3;

				// make sure it isn't a surrogate pair
				if ((ch & 0xF800) == 0xD800)
					ch = badchar;

				ret += ch;
			}
			else if (s[i] < 0xF8)    // F0-F7: 4 byte codepoint
			{
				if ((s[i + 1] & 0xC0) != 0x80)		throw 1;
				if ((s[i + 2] & 0xC0) != 0x80)		throw 2;
				if ((s[i + 3] & 0xC0) != 0x80)		throw 3;

				unsigned long ch =
					((s[i] & 0x07) << 18) |
					((s[i + 1] & 0x3F) << 12) |
					((s[i + 2] & 0x3F) << 6) |
					((s[i + 3] & 0x3F));
				i += 4;

				// make sure it isn't a surrogate pair
				if ((ch & 0xFFF800) == 0xD800)
					ch = badchar;

				if (ch < 0x10000)	// overlong encoding -- but technically possible
					ret += static_cast<wchar_t>(ch);
				else if (WCHAR_MAX < 0x110000)
				{
					// wchar_t is too small for 4 byte code point
					//  encode as UTF-16 surrogate pair

					ch -= 0x10000;
					ret += static_cast<wchar_t>((ch >> 10) | 0xD800);
					ret += static_cast<wchar_t>((ch & 0x03FF) | 0xDC00);
				}
				else
					ret += static_cast<wchar_t>(ch);
			}
			else                    // F8-FF: invalid
				throw 0;
		}
		catch (int skip)
		{
			if (!skip)
			{
				do
				{
					++i;
				} while ((s[i] & 0xC0) == 0x80);
			}
			else
				i += skip;
		}
	}

	return ret;
}