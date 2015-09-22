//game class functions and such
#include "Game.h"

//constructor
Game::Game()
{
	input = new Input();		//initialize keyboard input
	//additional initialization handled later in input->initialiazation()
	bPaused = false;
	graphics = NULL;
//	console = NULL;
	bInitialized = false;
	fFps = 100;
	incX = incY = incZ = 0;
	cameraX = cameraZ = 0;
	cameraY = 100;
	targetX = targetY = 0;
	targetZ = 0;
	upX = upZ = 0;
	upY = 1;
	incA = TURN_DIVISIONS / 4;
	theta = PI / 2;

	//cameraY = 50;
	//targetY = 1;

	//initialize lighting to 1 light
	for (int i = 0; i < 8; i++)
	{
		Light light;
		lighting.Lights[i] = light;
	}
	lighting.GlobalAmbient = DirectX::XMFLOAT4(.4, .4, .4, 1);

	lighting.Lights[0].Enabled = 1;
	lighting.Lights[0].LightType = PointLight;
	lighting.Lights[0].Color = DirectX::XMFLOAT4(1, 1, 0, 1);
	//lighting.Lights[0].SpotAngle = XMConvertToRadians(45.0f);
	lighting.Lights[0].ConstantAttenuation = 1.0f;
	lighting.Lights[0].LinearAttenuation = 0.005f;
	lighting.Lights[0].QuadraticAttenuation = 0.0f;
	lighting.Lights[0].Position = DirectX::XMFLOAT4( 0, 500, 0, 1);
	DirectX::XMVECTOR LightDirection0 = DirectX::XMVectorSet(-lighting.Lights[0].Position.x, -lighting.Lights[0].Position.y, -lighting.Lights[0].Position.z, 0.0f);
	LightDirection0 = DirectX::XMVector3Normalize(LightDirection0);
	XMStoreFloat4(&lighting.Lights[0].Direction, LightDirection0);

	lighting.Lights[1].Enabled = 1;
	lighting.Lights[1].LightType = PointLight;
	lighting.Lights[1].Color = DirectX::XMFLOAT4(1, 0, 1, 1);
	//lighting.Lights[0].SpotAngle = XMConvertToRadians(45.0f);
	lighting.Lights[1].ConstantAttenuation = 1.0f;
	lighting.Lights[1].LinearAttenuation = 0.005f;
	lighting.Lights[1].QuadraticAttenuation = 0.0f;
	lighting.Lights[1].Position = DirectX::XMFLOAT4(-2500, 500, -2500, 1);
	DirectX::XMVECTOR LightDirection1 = DirectX::XMVectorSet(-lighting.Lights[1].Position.x, -lighting.Lights[1].Position.y, -lighting.Lights[1].Position.z, 0.0f);
	LightDirection1 = DirectX::XMVector3Normalize(LightDirection1);
	XMStoreFloat4(&lighting.Lights[1].Direction, LightDirection1);

	lighting.Lights[2].Enabled = 1;
	lighting.Lights[2].LightType = PointLight;
	lighting.Lights[2].Color = DirectX::XMFLOAT4(0, 1, 0, 1);
	//lighting.Lights[2].SpotAngle = XMConvertToRadians(45.0f);
	lighting.Lights[2].ConstantAttenuation = 1.0f;
	lighting.Lights[2].LinearAttenuation = 0.005f;
	lighting.Lights[2].QuadraticAttenuation = 0.0f;
	lighting.Lights[2].Position = DirectX::XMFLOAT4(-2500, 500, 2500, 1);
	DirectX::XMVECTOR LightDirection2 = DirectX::XMVectorSet(-lighting.Lights[2].Position.x, -lighting.Lights[2].Position.y, -lighting.Lights[2].Position.z, 0.0f);
	LightDirection2 = DirectX::XMVector3Normalize(LightDirection2);
	XMStoreFloat4(&lighting.Lights[2].Direction, LightDirection2);

	lighting.Lights[3].Enabled = 1;
	lighting.Lights[3].LightType = PointLight;
	lighting.Lights[3].Color = DirectX::XMFLOAT4(1, 1, 1, 1);
	//lighting.Lights[3].SpotAngle = XMConvertToRadians(45.0f);
	lighting.Lights[3].ConstantAttenuation = 1.0f;
	lighting.Lights[3].LinearAttenuation = 0.005f;
	lighting.Lights[3].QuadraticAttenuation = 0.0f;
	lighting.Lights[3].Position = DirectX::XMFLOAT4(2500, 500, -2500, 1);
	DirectX::XMVECTOR LightDirection3 = DirectX::XMVectorSet(-lighting.Lights[3].Position.x, -lighting.Lights[3].Position.y, -lighting.Lights[3].Position.z, 0.0f);
	LightDirection3 = DirectX::XMVector3Normalize(LightDirection3);
	XMStoreFloat4(&lighting.Lights[3].Direction, LightDirection3);

	lighting.Lights[4].Enabled = 1;
	lighting.Lights[4].LightType = PointLight;
	lighting.Lights[4].Color = DirectX::XMFLOAT4(0, 0, 1, 1);
	//lighting.Lights[4].SpotAngle = XMConvertToRadians(45.0f);
	lighting.Lights[4].ConstantAttenuation = 1.0f;
	lighting.Lights[4].LinearAttenuation = 0.005f;
	lighting.Lights[4].QuadraticAttenuation = 0.0f;
	lighting.Lights[4].Position = DirectX::XMFLOAT4(2500, 500, 2500, 1);
	DirectX::XMVECTOR LightDirection4 = DirectX::XMVectorSet(-lighting.Lights[4].Position.x, -lighting.Lights[4].Position.y, -lighting.Lights[4].Position.z, 0.0f);
	LightDirection4 = DirectX::XMVector3Normalize(LightDirection4);
	XMStoreFloat4(&lighting.Lights[4].Direction, LightDirection4);
}

Game::~Game()
{
	deleteAll();		//clear memory used
	ShowCursor(true);	//shows the cursor
	//deconstruct collisionGrid
/*	for (int i = 0; i < GRID_HASH_SIZE; i++)
	{
		if (collisionGrid[i].next != NULL)
			delete collisionGrid[i].next;
		if (collisionGrid[i].object != NULL)
			delete collisionGrid[i].object;
		if (collisionGrid[i].npc != NULL)
			delete collisionGrid[i].npc;
	}*/
}

LRESULT Game::messageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(bInitialized)
	{
		switch(msg)
		{
			case WM_DESTROY:						//kill program
				PostQuitMessage(0);
				return 0;
			case WM_KEYDOWN: case WM_SYSKEYDOWN:	//down key
				input->keyDown(wParam);
				return 0;
			case WM_KEYUP: case WM_SYSKEYUP:		//up key
				input->keyUp(wParam);
				return 0;
			case WM_CHAR:							//character entered
				input->keyIn(wParam);
				return 0;
			case WM_MOUSEMOVE:						//mouse moved
				input->mouseIn(lParam);
				return 0;
			case WM_INPUT:							//raw mouse data
				input->mouseRawIn(lParam);
				return 0;
			case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK://mouse left button down
				input->setMouseLButton(true);
				input->mouseIn(lParam);
				return 0;
			case WM_LBUTTONUP:						//mouse left button released	
				input->setMouseLButton(false);
				input->mouseIn(lParam);
				return 0;
			case WM_MBUTTONDOWN:					//mouse middle button down
				input->setMouseMButton(true);
				input->mouseIn(lParam);
				return 0;
			case WM_MBUTTONUP:						//mouse middle button released	
				input->setMouseMButton(false);
				input->mouseIn(lParam);
				return 0;
			case WM_RBUTTONDOWN:					//mouse right button down
				input->setMouseRButton(true);
				input->mouseIn(lParam);
				return 0;
			case WM_RBUTTONUP:						//mouse right button released	
				input->setMouseRButton(false);
				input->mouseIn(lParam);
				return 0;
			case WM_XBUTTONDOWN: case WM_XBUTTONUP:	//mouse x button down or released	
				input->setMouseXButton(wParam);
				input->mouseIn(lParam);
				return 0;
			case WM_MOUSEWHEEL:                     //mouse wheel move
                input->mouseWheelIn(wParam);
                return 0;
			case WM_DEVICECHANGE:					//check for controllers
				input->checkControllers();
				return 0;
			//case(s) here for bFullScreen change and window resize
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//initialize game and timer
void Game::initialize(HWND hw, UINT w, UINT h, bool f)
{
	hwnd = hw;
	//initialize graphics
	graphics = new Graphics();
	graphics->initialize(hwnd, w, h, f);
	//initialize input
	input->initialize(hwnd, true);
	//initialize sound engine
	sound = new Sound;
	/*if (*WAVE_BANK != '\0' && *SOUND_BANK != '\0')  // if sound files defined
    {
        if( FAILED( hr = sound->initialize() ) )
        {
            if( hr == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
                throw(GameError(gameErrorNS::FATAL_ERROR, L"Failed to initialize sound system, sound files not found."));
            else
                throw(GameError(gameErrorNS::FATAL_ERROR, L"Failed to initialize sound system."));
        }
    }*/
	//initialize the command console
//	console = new Console();
//	console->initialize(graphics, input);
//	console->print("__________Command Console__________");
	//initialize timer
	gameTimer = new Timer();

	graphics->camera.initialize();
	meshHandler.initialize(graphics);

	bInitialized = true;
	ShowCursor(false);
}

//render game items
void Game::renderGame()
{
	graphics->beginRender(&lighting);
//	if(SUCCEEDED(graphics->beginRender()))
//	{
		render();
//		console->draw();
		graphics->endRender();
//	}
}

void Game::run(HWND hwnd)
{
	if(graphics == NULL)
		return;

	gameTimer->tick();
	//get fFrameTime
	fFrameTime = gameTimer->getDeltaTime();

	//CPU power saving
/*	if (fFrameTime < MIN_FRAME_TIME)
	{
		sleepTime = (DWORD)((MIN_FRAME_TIME - fFrameTime)*1000);
		timeBeginPeriod(1);
		Sleep(sleepTime);
		timeBeginPeriod(1);
		return;
	}*/
	if(fFrameTime > 0.0)
		fFps = (fFps*0.99f) + (0.01f/fFrameTime);  //avg fFps
	
	//limit fFrameTime if frame rate is slow
	if(fFrameTime > MAX_FRAME_TIME)
		fFrameTime = MAX_FRAME_TIME;

	if (!bPaused)
	{
		ai();
		update();
		input->vibrateControllers(fFrameTime);
	}
	renderGame();

	//check for console command key entry
//   if (input->getCharIn() == CONSOLE_KEY)
//	if(input->wasKeyPressed(CONSOLE_KEY))
//    {
//        input->clearCharIn();       // clear last char
//        console->toggleVisibility();
//        bPaused = console->getVisible(); // pause game when console is visible
//    }
//    consoleCommand();               // process user entered console command

	input->checkControllers();
	input->readControllers();

	//sound->run();

	if (input->wasKeyPressed(VK_ESCAPE))
	{
//		deleteAll();
		PostQuitMessage(0);
	}
//		bPaused = !bPaused;

	input->clear(inputNS::KEYS_PRESSED);
	if(input->getTextIn().length() >= 50)
		input->clear(inputNS::TEXT_IN);
}

void Game::update()
{
	
}

void Game::updateCollisionGrid()
{
	long *index;
	GridCell *workingCell;
	

}

void Game::emptyCollisionGrid()
{
	for (int i = 0; i < GRID_HASH_SIZE; i++)
		clearGridContents(&collisionGrid[i]);
}

void Game::clearGridContents(GridContents *g)
{
	if (g->next != NULL)
		clearGridContents(g->next);
	g->next = NULL;
	if (g->object != NULL)
		clearGridObject(g->object);
	g->object = NULL;
	if (g->npc != NULL)
		clearGridNpc(g->npc);
	g->npc = NULL;
}

void Game::clearGridObject(ObjectPtrList *o)
{
	if (o->next != NULL)
		clearGridObject(o->next);
	o->next = NULL;
	o->objectPtr = NULL;
}

void Game::clearGridNpc(NpcPtrList *n)
{
	if (n->next != NULL)
		clearGridNpc(n->next);
	n->next = NULL;
	n->npcPtr = NULL;
}

void Game::geometryCollide()
{
	DirectX::XMFLOAT3 collisionVector; //holds difference between edges of collision. No collision on the axis will be zero
	long *index;
	GridCell *workingCell;
	GridContents *workingContents;
	ObjectPtrList *workingObjectList;

	//check player vs environment
	/*index = player.getGridLoc();
	workingCell = player.getGridCell();

	for (int i = 0; i <= 7; i++)
	{
		//if there are no more gridCells occupied by the player, break
		if (index[i] == -1)
			break;

		workingContents = &collisionGrid[index[i]];

		//make sure we're in the correct container
		while (workingCell->x != workingContents->gridCell.x || workingCell->y != workingContents->gridCell.y
			|| workingCell->z != workingContents->gridCell.z)
		{
			if (workingContents->next != NULL)
			{
				workingContents = workingContents->next;
			}
			else
			{
				throw(GameError(gameErrorNS::FATAL_ERROR, L"Error with grid collision system - bad hash information."));
			}
		}

		workingObjectList = workingContents->object;
		while (workingObjectList != NULL)
		{
			if (player.collidesWith(workingObjectList->objectPtr, &collisionVector))
			{
				//collision code here
				//reverse along moveVector to undo collision
				
				//reverse y movement always?
				player.setY(player.getY() + collisionVector.y);
				//try greatest movement vector
				if (player.moveVector.x > player.moveVector.z)
					player.setX(player.getX() + collisionVector.x);
				else
					player.setZ(player.getZ() + collisionVector.z);
			}

			workingObjectList = workingObjectList->next;
		}
		workingCell = workingCell->next;
	}//end for */
}

void Game::objectCollide()
{

}

void Game::ai()
{

}

void Game::render()
{
	//implement in child class
}

//file I/O functions
void Game::loadConfig()
{
	//implement in child class
}

void Game::saveConfig()
{
	//implement in child class
}

int Game::convertString(std::string s)
{
	std::stringstream convert(s);
	int value;
	convert >> value;

	return value;
}

void Game::releaseAll()
{
//	safeOnLostDevice(console);
}

void Game::resetAll()
{
//	safeOnResetDevice(console);
}

void Game::deleteAll()
{
	emptyCollisionGrid();
    releaseAll();
	safeDelete(graphics);
	safeDelete(input);
	safeDelete(sound);
	safeDelete(gameTimer);
    bInitialized = false;
}

bool Game::loadLevel(int level)
{
	//implement in child class
	return false;
}

