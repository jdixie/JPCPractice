#ifndef _IMAGE_H
#define _IMAGE_H
#define WIN32_LEAN_AND_MEAN

#include "Graphics.h"
#include "input.h"
#include "constants.h"
#include "textureManager.h"

class Image
{
protected:
	Graphics *graphics;

	int width;		//width of the image
	int height;		//height of the image

	//vertex info
	ImageInfo imageInfo;

	float frameDelay;	//time between frame animations
	float animTimer;	//animation timer

	bool visible;		//whether the image is to be drawn
	bool looping;		//true for looping animation
	bool flipHorizontal;//whether to flip image texture about the y axis
	bool flipVertical;	//whether to flip image about the x axis

	//used to cap edges of map back when it was 2d, may not need these
	float xMoveCap;
	float yMoveCap;
	float zMoveCap;

public:
	//temporary movement variables to check if move is ok before finalizing
//	float tempX;
//	float tempY;
//	float tempZ;
	DirectX::XMFLOAT3 moveVector;

	//constructor/destructor
	Image();
	virtual ~Image();

	//initializer
	virtual bool initialize(Graphics *g, int w, int h,  float sc);

	//accessor functions
	virtual float getX() { return imageInfo.x; }
	virtual float getY() { return imageInfo.y; }
	virtual float getZ() { return imageInfo.z; }
	virtual float getRx() { return imageInfo.rx; }
	virtual float getRy() { return imageInfo.ry; }
	virtual float getRz() { return imageInfo.rz; }
//	virtual float getTempX() { return tempX; }
//	virtual float getTempY() { return tempY; }
//	virtual float getTempZ() { return tempZ; }
//	virtual float getRotationAngleZ() { return imageInfo.rotAngleZ; }
	virtual float getScale() { return imageInfo.scale; }
	virtual float *getBlendFactors() { return imageInfo.blendFactors; }
	virtual ImageInfo *getImageInfo() { return &imageInfo; }

	virtual bool isVisible() { return visible; }
	virtual bool isLooping() { return looping; }
	virtual bool isFlippedHorizontal() { return flipHorizontal; }
	virtual bool isFlippedVertical() { return flipVertical; }

	//setter functions
	virtual void setX(float f) { imageInfo.x = f; }
	virtual void setY(float f) { imageInfo.y = f; }
	virtual void setZ(float f) { imageInfo.z = f; }
//	virtual void setTempX(float f) { tempX = f; }
//	virtual void setTempY(float f) { tempY = f; }
//	virtual void setTempZ(float f) { tempZ = f; }
	virtual void setCoords(float tx, float ty, float tz)
	{
		imageInfo.x = tx;
		imageInfo.y = ty;
		imageInfo.z = tz;
	}

	//replace when quaternion is done
	virtual void rotate(float a, float xr, float yr, float zr)
	{
		imageInfo.rotation.makeRotate(a, xr, yr, zr);
	}

	virtual void setScale(float f) { imageInfo.scale = f; }

	virtual void setVisible(bool b) { visible = b; }
	virtual void setLooping(bool b) { looping = b; }
	virtual void setFlipHorizontal(bool b) { flipHorizontal = b; }
	virtual void setFlipVertical(bool b) { flipVertical = b; }
	virtual void setBlendFactors(float bf[4]) 
	{
		imageInfo.blendFactors[0] = bf[0];
		imageInfo.blendFactors[1] = bf[1];
		imageInfo.blendFactors[2] = bf[2];
		imageInfo.blendFactors[3] = bf[3];
	}
	virtual void setColor(float c[4]) 
	{ 
		imageInfo.color[0] = c[0];
		imageInfo.color[1] = c[1];
		imageInfo.color[2] = c[2];
		imageInfo.color[3] = c[3];
	}

	virtual void setColorVertices();
	virtual void setVertices();
	virtual void draw();
	 
	virtual void update(float frameTime);
/*	virtual void finalizeMovement()
	{
		imageInfo.x = tempX;
		imageInfo.y = tempY;
		imageInfo.z = tempZ;
	}*/
};

#endif