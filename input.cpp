#include "input.h"

//constructor
Input::Input()
{
	for(size_t i = 0; i < inputNS::KEYS_ARRAY; i++)
	{
		keysDown[i] = false;
		keysPressed[i] = false;
	}
	newLine = true;
	textIn = "";
	charIn = 0;
	mouseX = 0;
	mouseY = 0;
	mouseRawX = 0;
	mouseRawY = 0;
	mouseLButton = false;
	mouseRButton = false;
	mouseMButton = false;
	mouseX1Button = false;
	mouseX2Button = false;
	for(int i = 0; i < MAX_CONTROLLERS; i++)
	{
		controllers[i].vibrateTimeLeft = 0;
		controllers[i].vibrateTimeRight = 0;
	}
	thumbstickDeadzone = GAMEPAD_THUMBSTICK_DEADZONE;
	triggerDeadzone = GAMEPAD_TRIGGER_DEADZONE;
}

//destructor
Input::~Input()
{
	if(mouseCaptured)
		ReleaseCapture();
}

//initialize mouse and controller input.
//throws GameError
//pre: hwnd = window handle
//      capture = true to capture mouse.
void Input::initialize(HWND hwnd, bool capture)
{
	try
	{
		mouseCaptured = capture;
		//register HD mouse
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		Rid[0].dwFlags = RIDEV_INPUTSINK;
		Rid[0].hwndTarget = hwnd;
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
		if (mouseCaptured)
			SetCapture(hwnd);
		//clear controller state
		ZeroMemory(controllers, sizeof(ControllerState) * MAX_CONTROLLERS);
		checkControllers();
	}
	catch(...)
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error initializing input system"));
	}
}

//save key down state
void Input::keyDown(WPARAM wParam)
{
	if(wParam < inputNS::KEYS_ARRAY)
	{
		keysDown[wParam] = true;
		keysPressed[wParam] = true;
	}
}

//save key up state
void Input::keyUp(WPARAM wParam)
{
	if(wParam < inputNS::KEYS_ARRAY)
		keysDown[wParam] = false;
}

//save the char just entered in textIn string
void Input::keyIn(WPARAM wParam)
{
	if(newLine)
	{
		textIn.clear();
		newLine = false;
	}
	if(wParam == '\b')
	{
		if(textIn.length() > 0)
			textIn.erase(textIn.size()-1);
	}
	else
	{
		textIn += wParam;
		charIn = wParam;
	}
	if((char)wParam == '\r')
		newLine = true;
}

//returns true if the specified VIRTUAL KEY is down, otherwise false.
bool Input::isKeyDown(UCHAR vkey) const
{
	if(vkey < inputNS::KEYS_ARRAY)
		return keysDown[vkey];
	else
		return false;
}

//return true if the specified VIRTUAL KEY has been pressed in the most recent frame.
//key presses are erased at the end of each frame.
bool Input::wasKeyPressed(UCHAR vkey) const
{
	if(vkey < inputNS::KEYS_ARRAY)
		return keysPressed[vkey];
	else
		return false;
}

//return true if any key was pressed in the most recent frame.
//key presses are erased at the end of each frame.
bool Input::anyKeyPressed() const
{
	for (size_t i = 0; i < inputNS::KEYS_ARRAY; i++)
		if(keysPressed[i] == true)
			return true;
	return false;
}

//clear the specified key press
void Input::clearKeyPress(UCHAR vkey)
{
	if(vkey < inputNS::KEYS_ARRAY)
		keysPressed[vkey] = false;
}


//clear specified input buffers where what is any combination of
//KEYS_DOWN, KEYS_PRESSED, MOUSE, TEXT_IN or KEYS_MOUSE_TEXT.
//use OR '|' operator to combine parmeters.
void Input::clear(UCHAR toClear)
{
	if(toClear & inputNS::KEYS_DOWN)
		for(size_t i = 0; i < inputNS::KEYS_ARRAY; i++)
			keysDown[i] = false;
	if(toClear & inputNS::KEYS_PRESSED)
		for(size_t i = 0; i < inputNS::KEYS_ARRAY; i++)
			keysPressed[i] = false;
	if(toClear & inputNS::MOUSE)
	{
		mouseX = 0;
		mouseY = 0;
		mouseRawX = 0;
		mouseRawY = 0;
	}
	if(toClear & inputNS::TEXT_IN)
	{
		clearTextIn();
		clearCharIn();
	}
}

//reads mouse screen position into mouseX, mouseY
void Input::mouseIn(LPARAM lParam)
{
	mouseX = GET_X_LPARAM(lParam);
	mouseY = GET_Y_LPARAM(lParam);
}

//wheel data is in multiples of 120. positivie values are "scroll up"
void Input::mouseWheelIn(WPARAM wParam)
{
    mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
}

//reads raw mouse data into mouseRawX, mouseRawY
//compatible with a high-definition mouse
void Input::mouseRawIn(LPARAM lParam)
{
	UINT dwSize = 40;
	static BYTE lpb[40];

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
	RAWINPUT *raw = (RAWINPUT*)lpb;
	if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		mouseRawX = raw->data.mouse.lLastX;
		mouseRawY = raw->data.mouse.lLastY;
	}
}

//update connection status of game controllers.
void Input::checkControllers()
{
	DWORD result;
    for( DWORD i = 0; i < MAX_CONTROLLERS; i++)
    {
        result = XInputGetState(i, &controllers[i].state);
        if(result == ERROR_SUCCESS)
            controllers[i].connected = true;
        else
            controllers[i].connected = false;
    }
}

//save input from connected game controllers.
void Input::readControllers()
{
	DWORD result;
	for(DWORD i = 0; i < MAX_CONTROLLERS; i++)
		if(controllers[i].connected)
		{
			result = XInputGetState(i, &controllers[i].state);
			if(result == ERROR_DEVICE_NOT_CONNECTED)
				controllers[i].connected = false;
		}
}

// Vibrates the connected controllers for the desired time.
void Input::vibrateControllers(float frameTime)
{
	for(int i = 0; i< MAX_CONTROLLERS; i++)
		if(controllers[i].connected)
		{
			controllers[i].vibrateTimeLeft -= frameTime;
			if(controllers[i].vibrateTimeLeft < 0)
			{
				controllers[i].vibrateTimeLeft = 0;
				controllers[i].vibration.wLeftMotorSpeed = 0;
			}
			controllers[i].vibrateTimeRight -= frameTime;
			if(controllers[i].vibrateTimeRight < 0)
			{
				controllers[i].vibrateTimeRight = 0;
				controllers[i].vibration.wRightMotorSpeed = 0;
			}
			XInputSetState(i, &controllers[i].vibration);
		}
}

BYTE Input::getGamepadLeftTrigger(UINT n) 
{
    BYTE value = getGamepadLeftTriggerUndead(n);
    if(value > triggerDeadzone)             // if > dead zone
        //adjust magnitude relative to the end of the dead zone
        value = (value-triggerDeadzone)*255/
        (255-triggerDeadzone);
    else                                    // else, < dead zone
        value = 0;
    return value;
}

BYTE Input::getGamepadRightTrigger(UINT n) 
{
    BYTE value = getGamepadRightTriggerUndead(n);
    if(value > triggerDeadzone)    // if > dead zone
        //adjust magnitude relative to the end of the dead zone
        value = (value-triggerDeadzone)*255/
        (255-triggerDeadzone);
    else                                    // else, < dead zone
        value = 0;
    return value;
}

SHORT Input::getGamepadThumbLX(UINT n) 
{
    int x = getGamepadThumbLXUndead(n);
    if(x > thumbstickDeadzone) // if +x outside dead zone
        //adjust x relative to the deadzone and scale to 0 through 32767
        x = (x-thumbstickDeadzone)*32767/
        (32767-thumbstickDeadzone);
    else if(x < -thumbstickDeadzone)   // if -x outside dead zone
        //adjust y relative to the deadzone and scale to 0 through -32767
        x = (x+thumbstickDeadzone)*32767/
        (32767-thumbstickDeadzone);
    else        // else, x inside dead zone
        x = 0;  // return 0
    return static_cast<SHORT>(x);
}

SHORT Input::getGamepadThumbLY(UINT n) 
{
    int y = getGamepadThumbLYUndead(n);
    if(y > thumbstickDeadzone) // if +y outside dead zone
        //adjust magnitude relative to the end of the dead zone
        y = (y-thumbstickDeadzone)*32767/
        (32767-thumbstickDeadzone);
    else if(y < -thumbstickDeadzone)   // if -y outside dead zone
        //adjust magnitude relative to the end of the dead zone
        y = (y+thumbstickDeadzone)*32767/
        (32767-thumbstickDeadzone);
    else        // else, y inside dead zone
        y = 0;  // return 0
    return static_cast<SHORT>(y);
}

SHORT Input::getGamepadThumbRX(UINT n) 
{
    int x = getGamepadThumbRXUndead(n);
    if(x > thumbstickDeadzone) // if +x outside dead zone
        //adjust magnitude relative to the end of the dead zone
        x = (x-thumbstickDeadzone)*32767/
        (32767-thumbstickDeadzone);
    else if(x < -thumbstickDeadzone)   // if -x outside dead zone
        //adjust magnitude relative to the end of the dead zone
        x = (x+thumbstickDeadzone)*32767/
        (32767-thumbstickDeadzone);
    else        // else, x inside dead zone
        x = 0;  // return 0
    return static_cast<SHORT>(x);
}

SHORT Input::getGamepadThumbRY(UINT n) 
{
    int y = getGamepadThumbRYUndead(n);
    if(y > thumbstickDeadzone) // if +y outside dead zone
        //adjust magnitude relative to the end of the dead zone
        y = (y-thumbstickDeadzone)*32767/
        (32767-thumbstickDeadzone);
    else if(y < -thumbstickDeadzone)   // if -y outside dead zone
        //adjust magnitude relative to the end of the dead zone
        y = (y+thumbstickDeadzone)*32767/
        (32767-thumbstickDeadzone);
    else        // else, y inside dead zone
        y = 0;  // return 0
    return static_cast<SHORT>(y);
}