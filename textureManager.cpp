#include "textureManager.h"

//constructor
TextureManager::TextureManager()
{
//	texture = NULL;
//	iTexture = NULL;
	textureMapSRV = NULL;
    width = 0;
    height = 0;
    file = NULL;
    graphics = NULL;
    initialized = false;
}
//destructor
TextureManager::~TextureManager()
{
	textureMapSRV->Release();
}
//load texture
bool TextureManager::initialize(Graphics *graph, const wchar_t *f)
{
	graphics = graph;
	file = f;
		
	//load and confirm texture
//	hr = D3DX11CreateShaderResourceViewFromFile(graphics->getDevice(), file, 0, 0, &textureMapSRV, 0);
//	if (FAILED(hr))
//		throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating texture resource."));

	textureMapSRV = graphics->createTextureSRV(file);

	//get texture dimensions
		

	initialized = true;
	return true;
}

//dump texture if graphics device lost
void TextureManager::onLostDevice()
{
	if(!initialized)
		return;
//	safeRelease(texture);
//	safeRelease(iTexture);
}
//load texture on device reset
void TextureManager::onResetDevice()
{
	if(!initialized)
		return;
//	graphics->loadTexture(file);
}