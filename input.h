//input class and variales
//shorter functions are written out here instead of input.cpp

#ifndef _INPUT_H                 
#define _INPUT_H                
#define WIN32_LEAN_AND_MEAN

class Input;

#include <windows.h>
#include <WindowsX.h>
#include <string>
#include <XInput.h>
#include "constants.h"
#include "gameError.h"


//for high-definition mouse
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC      ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE     ((USHORT) 0x02)
#endif


namespace inputNS
{
    const int KEYS_ARRAY = 256;     //size of key arrays
    
    //what values for clear(), bit flag
    const UCHAR KEYS_DOWN = 1;
    const UCHAR KEYS_PRESSED = 2;
    const UCHAR MOUSE = 4;
    const UCHAR TEXT_IN = 8;
    const UCHAR KEYS_MOUSE_TEXT = KEYS_DOWN + KEYS_PRESSED + MOUSE + TEXT_IN;
}

const DWORD GAMEPAD_THUMBSTICK_DEADZONE = (DWORD)(0.20f * 0X7FFF);    //default to 20% of range as deadzone
const DWORD GAMEPAD_TRIGGER_DEADZONE = 30;                      //trigger range 0-255
const DWORD MAX_CONTROLLERS = 4;                                //maximum number of controllers supported by XInput

//bit corresponding to gamepad button in state.Gamepad.wButtons
const DWORD GAMEPAD_DPAD_UP        = 0x0001; //1
const DWORD GAMEPAD_DPAD_DOWN      = 0x0002; //2
const DWORD GAMEPAD_DPAD_LEFT      = 0x0004; //4
const DWORD GAMEPAD_DPAD_RIGHT     = 0x0008; //8
const DWORD GAMEPAD_START_BUTTON   = 0x0010; //16
const DWORD GAMEPAD_BACK_BUTTON    = 0x0020; //32
const DWORD GAMEPAD_LEFT_THUMB     = 0x0040; //64
const DWORD GAMEPAD_RIGHT_THUMB    = 0x0080; //128
const DWORD GAMEPAD_LEFT_SHOULDER  = 0x0100; //256
const DWORD GAMEPAD_RIGHT_SHOULDER = 0x0200; //512
const DWORD GAMEPAD_A              = 0x1000; //4096
const DWORD GAMEPAD_B              = 0x2000; //8192
const DWORD GAMEPAD_X              = 0x4000; //16384
const DWORD GAMEPAD_Y              = 0x8000; //32768

struct ControllerState
{
    XINPUT_STATE        state;
    XINPUT_VIBRATION    vibration;
    float               vibrateTimeLeft;    //mSec
    float               vibrateTimeRight;   //mSec
    bool                connected;
};

class Input
{
private:
    bool keysDown[inputNS::KEYS_ARRAY];     //true if specified key is down
    bool keysPressed[inputNS::KEYS_ARRAY];  //true if specified key was pressed
    std::string textIn;                         //user entered text
    char charIn;                                //last character entered
    bool newLine;                               //true on start of new line
    int  mouseX, mouseY;                        //mouse screen coordinates
    int  mouseRawX, mouseRawY;                  //high-definition mouse data
	int  mouseWheel;                            //mouse wheel movement
    RAWINPUTDEVICE Rid[1];                      //for high-definition mouse
    bool mouseCaptured;                         //true if mouse captured
    bool mouseLButton;                          //true if left mouse button down
    bool mouseMButton;                          //true if middle mouse button down
    bool mouseRButton;                          //true if right mouse button down
    bool mouseX1Button;                         //true if X1 mouse button down
    bool mouseX2Button;                         //true if X2 mouse button down
    ControllerState controllers[MAX_CONTROLLERS];    //state of controllers
	short thumbstickDeadzone;
    short triggerDeadzone;

public:
    //constructor
    Input();

    //destructor
    virtual ~Input();

    //initialize mouse and controller input.
    //throws GameError
    //pre: hwnd = window handle
    //      capture = true to capture mouse.
    void initialize(HWND hwnd, bool capture);

    //save key down state
    void keyDown(WPARAM wParam);

    //save key up state
    void keyUp(WPARAM wParam);

    //save the char just entered in textIn string
    void keyIn(WPARAM wParam);

    //returns true if the specified VIRTUAL KEY is down, otherwise false.
    bool isKeyDown(UCHAR vkey) const;

    //return true if the specified VIRTUAL KEY has been pressed in the most recent frame.
    //key presses are erased at the end of each frame.
    bool wasKeyPressed(UCHAR vkey) const;

    //return true if any key was pressed in the most recent frame.
    //key presses are erased at the end of each frame.
    bool anyKeyPressed() const;

    //clear the specified key press
    void clearKeyPress(UCHAR vkey);

    //clear specified input buffers where what is any combination of
    //KEYS_DOWN, KEYS_PRESSED, MOUSE, TEXT_IN or KEYS_MOUSE_TEXT.
    //use OR '|' operator to combine parmeters.
    void clear(UCHAR keys);

    //clears key, mouse and text input data
    void clearAll() 
	{
		clear(inputNS::KEYS_MOUSE_TEXT);
	}

    //clear text input buffer
    void clearTextIn() 
	{
		textIn.clear();
	}

	//clear last character entered
    void clearCharIn()      {charIn = 0;}

    //return text input as a string
    std::string getTextIn() {return textIn;}

    //return last character entered
    char getCharIn()        {return charIn;}

    //reads mouse screen position into mouseX, mouseY
    void mouseIn(LPARAM);

    //reads raw mouse data into mouseRawX, mouseRawY
    //compatible with a high-definition mouse
    void mouseRawIn(LPARAM);

	//reads mouse wheel movement.
    void mouseWheelIn(WPARAM);

    //save state of mouse button
    void setMouseLButton(bool b) { mouseLButton = b; }

    //save state of mouse button
    void setMouseMButton(bool b) { mouseMButton = b; }

    //save state of mouse button
    void setMouseRButton(bool b) { mouseRButton = b; }

    //save state of mouse button
    void setMouseXButton(WPARAM wParam) {mouseX1Button = (wParam & MK_XBUTTON1) ? true:false;
                                         mouseX2Button = (wParam & MK_XBUTTON2) ? true:false;}
    //mouse X position
    int  getMouseX()        const { return mouseX; }

    //mouse Y position
    int  getMouseY()        const { return mouseY; }

    //return raw mouse X movement. Left is <0, Right is >0
    //compatible with high-definition mouse.
    int  getMouseRawX()     const { return mouseRawX; }

    //return raw mouse Y movement. Up is <0, Down is >0
    //compatible with high-definition mouse.
    int  getMouseRawY()     const { return mouseRawY; }

    //state of left mouse button.
    bool getMouseLButton()  const { return mouseLButton; }

    //state of middle mouse button.
    bool getMouseMButton()  const { return mouseMButton; }

    //state of right mouse button.
    bool getMouseRButton()  const { return mouseRButton; }

    //state of X1 mouse button.
    bool getMouseX1Button() const { return mouseX1Button; }

    //state of X2 mouse button.
    bool getMouseX2Button() const { return mouseX2Button; }

    //update connection status of game controllers.
    void checkControllers();

    //save input from connected game controllers.
    void readControllers();

	//set thumbstick deadzone
    void setThumbstickDeadzone(short dz) { thumbstickDeadzone = abs(dz); }

    //set trigger deadzone
    void setTriggerDeadzone(BYTE dz) { triggerDeadzone = dz; }

    //get thumbstick deadzone
    short getThumbstickDeadzone() { return thumbstickDeadzone; }

    //get trigger deadzone
    BYTE getTriggerDeadzone() { return static_cast<BYTE>(triggerDeadzone); }

    //state of specified game controller.
    const ControllerState* getControllerState(UINT c)
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return &controllers[c];
    }

    //state of controller c's buttons.
    const WORD getGamepadButtons(UINT c)
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return controllers[c].state.Gamepad.wButtons;
    }

    //return state of controller c's D-pad Up
    bool getGamepadDPadUp(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return ((controllers[c].state.Gamepad.wButtons & GAMEPAD_DPAD_UP) != 0);
    }

    //return state of controller c's D-pad Down.
    bool getGamepadDPadDown(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return ((controllers[c].state.Gamepad.wButtons & GAMEPAD_DPAD_DOWN) != 0);
    }

    //return state of controller c's D-pad Left.
    bool getGamepadDPadLeft(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return ((controllers[c].state.Gamepad.wButtons & GAMEPAD_DPAD_LEFT) != 0);
    }

    //return state of controller c's D-pad Right.
    bool getGamepadDPadRight(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_DPAD_RIGHT) != 0);
    }

    //return state of controller c's Start button.
    bool getGamepadStart(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_START_BUTTON) != 0);
    }

    //return state of controller c's Back button.
    bool getGamepadBack(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_BACK_BUTTON) != 0);
    }

    //return state of controller c's Left Thumb button.
    bool getGamepadLeftThumb(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_LEFT_THUMB) != 0);
    }

    //return state of controller c's Right Thumb button.
    bool getGamepadRightThumb(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_RIGHT_THUMB) != 0);
    }

    //return state of controller c's Left Shoulder button.
    bool getGamepadLeftShoulder(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_LEFT_SHOULDER) != 0);
    }

    //return state of controller c's Right Shoulder button.
    bool getGamepadRightShoulder(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_RIGHT_SHOULDER) != 0);
    }

    //return state of controller c's A button.
    bool getGamepadA(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_A) != 0);
    }

    //return state of controller c's B button.
    bool getGamepadB(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_B) != 0);
    }

    //return state of controller c's X button.
    bool getGamepadX(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_X) != 0);
    }

    //return state of controller c's Y button.
    bool getGamepadY(UINT c) 
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        return bool((controllers[c].state.Gamepad.wButtons & GAMEPAD_Y) != 0);
    }

    //return value of controller c's Left Trigger.
    BYTE getGamepadLeftTrigger(UINT c);

	//return value of controller n Left Trigger (0 through 255).
    //deadzone is not applied.
    BYTE getGamepadLeftTriggerUndead(UINT n) 
    {
        if(n > MAX_CONTROLLERS-1)
            n=MAX_CONTROLLERS-1;
        return controllers[n].state.Gamepad.bLeftTrigger;
    }

    //return value of controller c's Right Trigger.
    BYTE getGamepadRightTrigger(UINT c) ;

	//return value of controller n Right Trigger (0 through 255).
    //deadzone is not applied.
    BYTE getGamepadRightTriggerUndead(UINT n) 
    {
        if(n > MAX_CONTROLLERS-1)
            n=MAX_CONTROLLERS-1;
        return controllers[n].state.Gamepad.bRightTrigger;
    }

    //return value of controller c's Left Thumbstick x plane.
    SHORT getGamepadThumbLX(UINT c);

	//return value of controller n Left Thumbstick X (-32767 through 32767).
    //deadzone is not applied.
    SHORT getGamepadThumbLXUndead(UINT n) 
    {
        if(n > MAX_CONTROLLERS-1)
            n=MAX_CONTROLLERS-1;
        return controllers[n].state.Gamepad.sThumbLX;
    }

    //return value of controller c's Left Thumbstick y plane.
    SHORT getGamepadThumbLY(UINT c);

	//return value of controller n Left Thumbstick Y (-32768 through 32767).
    //deadzone is not applied.
    SHORT getGamepadThumbLYUndead(UINT n) 
    {
        if(n > MAX_CONTROLLERS-1)
            n=MAX_CONTROLLERS-1;
        return controllers[n].state.Gamepad.sThumbLY;
    }

    //return value of controller c's Right Thumbstick x plane.
    SHORT getGamepadThumbRX(UINT c);

	//return value of controller n Right Thumbstick X (-32768 through 32767).
    //deadzone is not applied.
    SHORT getGamepadThumbRXUndead(UINT n) 
    {
        if(n > MAX_CONTROLLERS-1)   // if invalid controller number
            n=MAX_CONTROLLERS-1;    // force valid
        return controllers[n].state.Gamepad.sThumbRX;
    }

    //return value of controller n Right Thumbstick y plane.
    SHORT getGamepadThumbRY(UINT c);

	//return value of controller n Right Thumbstick Y (-32768 through 32767).
    //deadzone is not applied.
    SHORT getGamepadThumbRYUndead(UINT n) 
    {
        if(n > MAX_CONTROLLERS-1)
            n=MAX_CONTROLLERS-1;
        return controllers[n].state.Gamepad.sThumbRY;
    }

    //vibrate controller c's left motor.
    //left is low frequency vibration.
    //speed 0=off, 65536=100 percent
    //sec is time to vibrate in seconds
    void gamePadVibrateLeft(UINT c, WORD speed, float sec)
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        controllers[c].vibration.wLeftMotorSpeed = speed;
        controllers[c].vibrateTimeLeft = sec;
    }

    //vibrate controller c's right motor.
    //right is high frequency vibration.
    //speed 0=off, 65536=100 percent
    //sec is time to vibrate in seconds
    void gamePadVibrateRight(UINT c, WORD speed, float sec)
    {
        if(c > MAX_CONTROLLERS-1)
            c=MAX_CONTROLLERS-1;
        controllers[c].vibration.wRightMotorSpeed = speed;
        controllers[c].vibrateTimeRight = sec;
    }

    // Vibrates the connected controllers for the desired time.
    void vibrateControllers(float frameTime);
};

#endif