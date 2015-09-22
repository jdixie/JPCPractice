//constants for 2d game framework
//additional constants added per game

#ifndef _CONSTANTS_H             
#define _CONSTANTS_H            
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "MeshHelper.h"

//macros
template <typename T>
inline void safeRelease(T& ptr)
{
    if (ptr)
    { 
        ptr->Release(); 
        ptr = NULL;
    }
}
#define SAFE_RELEASE safeRelease            // for backward compatiblility

// Safely delete pointer referenced item
template <typename T>
inline void safeDelete(T& ptr)
{
    if (ptr)
    { 
        delete ptr; 
        ptr = NULL;
    }
}
#define SAFE_DELETE safeDelete              // for backward compatiblility

// Safely delete pointer referenced array
template <typename T>
inline void safeDeleteArray(T& ptr)
{
    if (ptr)
    { 
        delete[] ptr; 
        ptr = NULL;
    }
}
#define SAFE_DELETE_ARRAY safeDeleteArray   // for backward compatiblility

// Safely call onLostDevice
template <typename T>
inline void safeOnLostDevice(T& ptr)
{
    if (ptr)
        ptr->onLostDevice(); 
}
#define SAFE_ON_LOST_DEVICE safeOnLostDevice    // for backward compatiblility

// Safely call onResetDevice
template <typename T>
inline void safeOnResetDevice(T& ptr)
{
    if (ptr)
        ptr->onResetDevice(); 
}
#define SAFE_ON_RESET_DEVICE safeOnResetDevice  // for backward compatiblility

#define TRANSCOLOR  SETCOLOR_ARGB(255,230,0,230)  // transparent color

//constants
//window parameters
const wchar_t CLASS_NAME[] = L"WinMain";
const wchar_t APP_TITLE[] = L"Engine";
const wchar_t WAVE_BANK[] = L"sound\\Win\\bgm.xwb";
const wchar_t SOUND_BANK[] = L"sound\\Win\\se.xsb";
const double PI = 3.14159265358979323846;
const float FRAME_RATE  = 100.0f;               // the target (or max) frame rate (frames/sec)
const float MIN_FRAME_RATE = 10.0f;             // the minimum frame rate
const float MIN_FRAME_TIME = 1.0f/FRAME_RATE;   // minimum desired time for 1 frame
const float MAX_FRAME_TIME = 1.0f/MIN_FRAME_RATE; // maximum time used in calculations

const wchar_t TESTMUSIC[] = L"testmusic";

const wchar_t PLAYER_IMAGE[] = L"sprites\\hero.png";
const int  PLAYER_START_FRAME = 0;            
const int  PLAYER_END_FRAME = 5;              
const float PLAYER_ANIMATION_DELAY = 0.1f;    
const float PLAYER_SPEED = 700;
const int PLAYER_STANDING_FRAME = 0;
const int PLAYER_RUNNING_START_FRAME = 1;
const int PLAYER_RUNNING_END_FRAME = 5;

//collision grid constants
const int GRID_UNIT = 20;			//dimension of 1 grid cell
const int GRID_WIDTH = 40;			//X grid units
const int GRID_HEIGHT = 40;			//Y grid units
const int GRID_DEPTH = 40;			//Z gid units
const int HASH_XM = 7;				//hash key x multiplier
const int HASH_YM = 11;				//hash key y multiplier
const int HASH_ZM = 37;				//hash key z multiplier
const int GRID_HASH_SIZE = 1024;	//grid hash table array size

//tactics grid constants
const int TACTICAL_GRID_SIDE_LENGTH = 50;

//key mappings, will remove once config support is fully in
const UCHAR CONSOLE_KEY		= VK_OEM_3;		//~ key		
const UCHAR ESC_KEY			= VK_ESCAPE;
const UCHAR ALT_KEY			= VK_MENU;
const UCHAR ENTER_KEY		= VK_RETURN;
const UCHAR MOVE_X_NEG		= 'A';
const UCHAR MOVE_X_POS		= 'D';
const UCHAR MOVE_Y_POS		= 'E';
const UCHAR MOVE_Y_NEG		= 'Q';
const UCHAR JUMP_KEY		= VK_SPACE;
const UCHAR MOVE_Z_NEG		= 'S';
const UCHAR MOVE_Z_POS		= 'W';
const UCHAR MOVE_FORWARD	= 'W';
const UCHAR MOVE_BACKWARD	= 'S';
const UCHAR TURN_RIGHT		= 'D';
const UCHAR TURN_LEFT		= 'A';
const UCHAR STRAFE_RIGHT	= 'E';
const UCHAR STRAFE_LEFT		= 'Q';
const UCHAR UP_ARROW		= VK_UP;
const UCHAR DOWN_ARROW		= VK_DOWN;
const UCHAR LEFT_ARROW		= VK_LEFT;
const UCHAR RIGHT_ARROW		= VK_RIGHT;
const UCHAR ROTATE_CW		= 'O';
const UCHAR ROTATE_CCW		= 'P';

const int TURN_DIVISIONS	= 120;


//for collision grid
class Object;
class Npc;

struct ObjectPtrList
{
	Object *objectPtr;
	ObjectPtrList *next;
	ObjectPtrList()
	{
		objectPtr = NULL;
		next = NULL;
	}
	ObjectPtrList(Object *object)
	{
		objectPtr = object;
		next = NULL;
	}
	~ObjectPtrList()
	{
		/*		if (next != NULL)
		delete next;
		objectPtr = NULL;*/
	}
};

struct NpcPtrList
{
	Npc *npcPtr;
	NpcPtrList *next;
	NpcPtrList(Npc *npc)
	{
		npcPtr = npc;
		next = NULL;
	}
	~NpcPtrList()
	{
		/*		if (next != NULL)
		delete next;
		npcPtr = NULL;*/
	}
};

struct GridCell //for collision grid
{
	long x, y, z;
	GridCell *next;

	GridCell()
	{
		x = -1; y = -1; z = -1; next = NULL;
	}
	GridCell(long xIn, long yIn, long zIn)
	{
		x = xIn; y = yIn; z = zIn; next = NULL;
	}
	~GridCell()
	{
		/*		if (next != NULL)
		delete next;*/
	}
};

struct GridContents
{
	GridCell gridCell;
	ObjectPtrList *object;
	NpcPtrList *npc;
	bool player;

	GridContents *next;
	//constructor?
	GridContents()
	{
		gridCell = GridCell(-1, -1, -1);
		object = NULL;
		npc = NULL;
		player = false;
		next = NULL;
	}
	GridContents(GridCell gc)
	{
		gridCell = gc;
		object = NULL;
		npc = NULL;
		player = false;
		next = NULL;
	}
	~GridContents()
	{
		/*		if (object != NULL)
		delete object;
		if (npc != NULL)
		delete npc;*/
	}
};
#endif