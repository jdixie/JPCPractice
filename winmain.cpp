#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinUser.h>
#include <stdlib.h>
//#include <crtdbg.h>
//#include <vld.h>
#include "Graphics.h"
#include "JPCPractice.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool CreateMainWindow(HWND &, HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);


JPCPractice *game = NULL;
HWND hwnd = NULL;

int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, 
				   int nCmdShow)
{
	//check for memory leak during debug builds
	/*#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif*/

	MSG msg = { NULL };
	game = new JPCPractice;

	//Create window
	if (!CreateMainWindow(hwnd, hInstance, nCmdShow))
		return 1;
	try
	{
		//create and initialize directx graphics, throws GameError
		game->initialize(hwnd);

		//main message loop
		while (msg.message != WM_QUIT)
		{
			//non blocking method for checking for windows messages
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				//decode and pass messages to winproc
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}else
				game->run(hwnd);
		}
		game->deleteAll();
		return msg.wParam;
	}
	catch(const GameError &err)
    {
		game->deleteAll();
        DestroyWindow(hwnd);
        MessageBox(NULL, err.getMessage(), L"Error", MB_OK);
    }
    catch(...)
    {
		game->deleteAll();
        DestroyWindow(hwnd);
        MessageBox(NULL, L"Unknown error occured in game.", L"Error", MB_OK);
    }
	SAFE_DELETE(game);
	return msg.wParam;
}

//window event function
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return(game->messageHandler(hwnd, msg, wParam, lParam));
}

//create window function
bool CreateMainWindow(HWND &hwnd, HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcx;

	//Parameters that describe main window
	wcx.cbSize = sizeof(wcx);				//size of structure
	wcx.style = CS_HREDRAW | CS_VREDRAW;	//redraw if size changes
	wcx.lpfnWndProc = WinProc;				//points to window procedure
	wcx.cbClsExtra = 0;						//no extra class memory
	wcx.cbWndExtra = 0;						//no extra window memory
	wcx.hInstance = hInstance;				//Handle to instance
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL,IDC_ARROW); //predefined arrow
	//wcx.hCursor = LoadCursorFromFile(L"S1.ani");
	//background brush
	wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcx.lpszMenuName = NULL;				//name of menu resource
	wcx.lpszClassName = CLASS_NAME;			//name of window class
	wcx.hIconSm = NULL;						//small class icon

	//register window class, return 0 on error
	if (RegisterClassEx(&wcx) == 0)
		return false;

	//set fullscreen vs windowed
	DWORD style;
//	if (FULLSCREEN)
//		style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
//	else
		style = WS_OVERLAPPED; //WINDOW;

	hwnd = CreateWindow(
		CLASS_NAME,
		APP_TITLE,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1024,
		768,
		(HWND)NULL,
		(HMENU)NULL,
		hInstance,
		(LPVOID)NULL);

	//error check window creation, exit if failed
	if (!hwnd)
		return false;
	
	//redraw window so client area = size wanted if running windowed
//	if(!FULLSCREEN)
//	{
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);  //get size of client area
		MoveWindow(hwnd,
			0,
			0,
			1024 + (1024 - clientRect.right),
			768 + (768 - clientRect.bottom),
			TRUE);
//	}

	//show window
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return true;
}