#ifndef _GEOMETRY_H
#define _GEOMETRY_H
#define WIN32_LEAN_AND_MEAN

#include <sstream>
#include <unordered_map>
#include "graphics.h"
#include "Vector3.h"

class MeshHandler
{
private:
	Graphics *graphics;
	std::unordered_map<std::wstring, MeshData> meshTable;	//hash table storing all loaded meshes
	std::unordered_map<std::wstring, MeshData>::iterator meshIterator;	//iterator results from meshTable search

	//data for determing rx, ry, and rz values of the image
	float fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ;

public:
	MeshHandler();
	~MeshHandler();
	void initialize(Graphics *g);
	HRESULT loadMesh(ImageInfo *image, const wchar_t *file);	//parse binary mesh input
	HRESULT LoadMaterialsFromMTL(const wchar_t *file, MeshData *tempMesh); //load material info from file
	void creatBasicLandscape(ImageInfo *image, int width, int height, int interval); //create basic land geometry
	void createGrid(float width, float depth, UINT m, UINT n, ImageInfo *image);
	void createSphere(float radius, float divisions, ImageInfo *image);
	void createRect(float length, float width, ImageInfo *image);
	void createRect(float length, float width, TextImageInfo *image);
};

#endif