#include "JPCPractice.h"

JPCPractice::JPCPractice()
{
	cameraX = 0.0f;
	cameraY = 1500.0f;
	cameraZ = 0.0f;
	ry = 75;
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

//	ShowCursor(false);

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
	loadLevel(0);
	geometry.at(0)->getImageInfo()->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(.4, .4, .4, 1);
	geometry.at(0)->rotate(90, 1, 0, 0);

	Object *sphere = new Object;
	sphere->initialize(graphics, 0, 0, 1);
	sphere->getImageInfo()->mesh = new MeshData;
	meshHandler.createSphere(150, 16, sphere->getImageInfo());
	sphere->getImageInfo()->mesh->textureMapSRV.push_back(graphics->createTextureSRV(L"mesh\\walls.png"));
	sphere->setY(100);
	sphere->setVertices();
	entity.push_back(sphere);

	graphics->camera.setCamType(CamType::FPS);
	graphics->camera.initialize();
}

void JPCPractice::update()
{
	/*if (input->isKeyDown(TURN_LEFT))//VK_LEFT))
		//incX -= 10;
		incA += 1;
	if (input->isKeyDown(TURN_RIGHT))//VK_RIGHT))
		//incX += 10;
		incA -= 1;
	if (input->isKeyDown(MOVE_Y_POS))//VK_UP))
		incY += 10;
	if (input->isKeyDown(MOVE_Y_NEG))//VK_DOWN))
		incY -= 10;
	if (input->isKeyDown(MOVE_BACKWARD))
	{
		//incZ -= 10;
		cameraX -= 10 * cos(theta);
		cameraZ -= 10 * sin(theta);
	}
	if (input->isKeyDown(MOVE_FORWARD))
	{
		//incZ += 10;
		cameraX += 10 * cos(theta);
		cameraZ += 10 * sin(theta);
	}*/
	if (input->isKeyDown(UP_ARROW))
		cameraZ += 15;
	if (input->isKeyDown(DOWN_ARROW))
		cameraZ -= 15;
	if (input->isKeyDown(RIGHT_ARROW))
		cameraX += 15;
	if (input->isKeyDown(LEFT_ARROW))
		cameraX -= 15;
	if (input->isKeyDown(MOVE_Y_POS))
		cameraY += 15;
	if (input->isKeyDown(MOVE_Y_NEG))
		cameraY -= 15;
	if (input->wasKeyPressed(ROTATE_CW))
		rxz += 45;
	if (input->wasKeyPressed(ROTATE_CCW))
		rxz -= 45;

	if (rxz == 360 || rxz == -360)
		rxz = 0;

	//cameraZ = incZ;
	//cameraY = incY;
	//cameraX = incX;

	if (incA == 2 * TURN_DIVISIONS)
		incA = 0;
	else if (incA == -2 * TURN_DIVISIONS)
		incA = 0;

	theta = (incA * PI) / TURN_DIVISIONS;

	targetX = cameraX + cos(theta);
	targetZ = cameraZ + sin(theta);

	//update collisionGrid - clear entries
	updateCollisionGrid();

	//update player, npc's, objects, and images to set their temporary movement variables
	//player.update(fFrameTime, fGameScale);



	//check fo collisions between player and geometry, npc and geometry, player and npc, objects and everything else
	//loop it to run 3-ish times to check for new collisions after adjustments
	//debug - call only on movement
	if (player.getVelocity().x != 0 || player.getVelocity().y != 0 || player.getVelocity().z != 0)
		geometryCollide();

	//	player.finalizeMovement();

	//update lighting
	lAngle += .05;
	if (lAngle >= 360)
		lAngle = 0;
	float sa = 500 * sin(lAngle);
	float ca = 500 * cos(lAngle);
	lighting.Lights[0].Position = DirectX::XMFLOAT4(sa, 500, ca, 1);
	lighting.Lights[1].Position = DirectX::XMFLOAT4(-2500 + sa, 500, -2500 + ca, 1);
	lighting.Lights[2].Position = DirectX::XMFLOAT4(-2500 + sa, 500, 2500 + ca, 1);
	lighting.Lights[3].Position = DirectX::XMFLOAT4(2500 + sa, 500, -2500 + ca, 1);
	lighting.Lights[4].Position = DirectX::XMFLOAT4(2500 + sa, 500, 2500 + ca, 1);
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
	graphics->camera.setPosition(cameraX, cameraY, cameraZ, ry, rxz);
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
}

void JPCPractice::loadConfig()
{
	std::string s;
	std::string v;
	std::ifstream configFile;
	configFile.open("Auramancer.cfg");

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

	//for testing purposes, be sure to remove
	Object *tempLand = new Object();
	tempLand->getImageInfo()->mesh = new MeshData();
	tempLand->getImageInfo()->mesh->textureMapSRV.push_back(graphics->createTextureSRV(L"mesh\\stone_floor.jpg"));
	meshHandler.createGrid(10000, 10000, 10, 10, tempLand->getImageInfo());
	tempLand->setCoords(0, 0, -1000);

	tempLand->initialize(graphics, 0, 0, fGameScale);
	tempLand->setVertices();
	geometry.push_back(tempLand);

	

	return true;
}