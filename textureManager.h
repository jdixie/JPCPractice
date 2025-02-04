#ifndef _TEXTUREMANAGER_H
#define _TEXTUREMANAGER_H
#define WIN32_LEAN_AND_MEAN
//texture loading and info, interacts with graphics class

#include <sstream>
#include <unordered_map>
#include "constants.h"
#include "graphics.h"

struct sTexture
{
	UINT width;
	UINT height;
	ID3D11ShaderResourceView *textureMapSRV;
	const wchar_t *file;
};

class TextureManager
{
private:
	//UINT       width;       // width of texture in pixels
    //UINT       height;      // height of texture in pixels
//	ID3D11Texture2D *texture;
	//std::vector<sTexture> textures;
	std::unordered_map<std::wstring, sTexture> textureTable;	//hash table storing all loaded textures
	std::unordered_map<std::wstring, sTexture>::iterator textureIterator;	//iterator results from texture search
    Graphics *graphics;     // save pointer to graphics
    bool    initialized;    // true when successfully initialized
    HRESULT hr;    

public:
	//constructor
	TextureManager();
	//destructor
	virtual ~TextureManager();

	virtual void initialize(Graphics *graph);
	//returns a pointer to the texture
	//ID3D11ShaderResourceView *getTextureSRV() const { return textureMapSRV; }
    //returns the texture width
    //UINT getWidth() const {return width;}
    //return the texture height
    //UINT getHeight() const {return height;}
	//load texture
	virtual ID3D11ShaderResourceView *loadTexture(const wchar_t *f);

	//dump texture if graphics device lost
	virtual void onLostDevice();
	//load texture on device reset
	virtual void onResetDevice();
};
#endif