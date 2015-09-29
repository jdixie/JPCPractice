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
	//geometry.at(0)->getImageInfo()->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(.4, .4, .4, 1);
	//geometry.at(0)->rotate(90, 1, 0, 0);

	/*Object *sphere = new Object;
	sphere->initialize(graphics, 0, 0, 1);
	sphere->getImageInfo()->mesh = new MeshData;
	meshHandler.createSphere(150, 16, sphere->getImageInfo());
	sphere->getImageInfo()->mesh->textureMapSRV.push_back(graphics->createTextureSRV(L"mesh\\walls.png"));
	sphere->setY(100);
	sphere->setVertices();
	entity.push_back(sphere);*/

	//set up play screens: vertically 0-4 are column 1 100-500, 5-9 are column 2 100-500, etc.
	//25-29 are the category screens across the top
	//center of each play screen, since looking down x axis, all x will be the same

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

	loadLevel(1);

	graphics->camera.setCamType(CamType::FPS);
	graphics->camera.initialize();
	graphics->camera.setPosition(0, 0, 0, 0, 90);
	rxz = 90;
}

void JPCPractice::update()
{
	if (input->getMouseLButton())
	{
		//compute ray for clicked spot  in view space
		float vx = ((2 * input->getMouseX() / nGameWidth) - 1) / graphics->getProjMatrixPosition(0);
		float vy = ((2 * input->getMouseY() / nGameHeight) - 1) / graphics->getProjMatrixPosition(11);
		float vz = 1;

		//input->setMouseLButton(false);
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
	//graphics->renderText(L"Test テスト");
	//graphics->renderText(L"Over or replace?");
	for (int i = 0; i < textImages.size(); i++)
		graphics->drawTextRect(textImages.at(i));
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

	/*TextImageInfo *temp = new TextImageInfo;
	temp->mesh = new MeshData;
	meshHandler.createRect(245, 145, temp);
	temp->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
	temp->text = L"Test";
	textImages.push_back(temp);
	graphics->InitD2DRectTexture(textImages.at(0));
	textImages.at(0)->z = 1049;
	textImages.at(0)->rotation.makeRotate(-90, 1, 0, 0);

	TextImageInfo *temp2 = new TextImageInfo;
	temp2->mesh = new MeshData;
	meshHandler.createRect(245, 145, temp2);
	temp2->mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
	temp2->text = L"Other";
	textImages.push_back(temp2);
	graphics->InitD2DRectTexture(textImages.at(1));
	textImages.at(1)->z = 1049;
	textImages.at(1)->y = 400;
	textImages.at(1)->rotation.makeRotate(-90, 1, 0, 0);*/

	//play screen reminder: vertically 0-4 are column 1 100-500, 5-9 are column 2 100-500, etc.
	//25-29 are the category screens across the top
	//center of each play screen, since looking down x axis, all x will be the same

	//load questions and text for screens
	switch (level)
	{
	case 2:
	case 1:
	default:
		float yc = 225;
		float xc = -500;
		for (int i = 0; i < 25; i++)
		{
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

			temp->text = L"Category";

			textImages.push_back(temp);
			graphics->InitD2DRectTexture(textImages.at(i), 144);
			textImages.at(i)->z = 1049;
			textImages.at(i)->rotation.makeRotate(-90, 1, 0, 0);

			textImages.at(i)->y = yc;
			textImages.at(i)->x = xc;
			xc += 250;
		}
		break;
	}

	return true;
}