#ifndef _CAMERA_H
#define _CAMERA_H
#define WIN32_LEAN_AND_MEAN

#include "Graphics.h"
#include "Quaternion.h"

//Current JPCPractice is ISO, adding ability for FPS for future use of the engine
enum CamType{
	FPS = 0,
	ISO = 1
};

class Camera
{
private:
	CamType camType;

	Quaternion lookAt;
	float xPos, yPos, zPos, xTar, yTar, zTar, upX, upY, upZ;
	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR target;
	DirectX::XMVECTOR up;
	DirectX::XMMATRIX worldRotation;
	//FPS
	//rotYAxis is about Y axis where +x is 0. rotXZplane is from XZ plane where toward +y is + angle
	float rotYAxis = 0, rotXZPlane = 0, length;

public:
	Camera()
	{
		camType = CamType::FPS;
		xPos = zPos = 0;
		yPos = 100;
		xTar = yTar = 0;
		zTar = 10;
		upX = upZ = 0;
		upY = 1;
		worldRotation = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
	}

	void initialize()
	{ 
		//xPos = xp; yPos = yp; zPos = zp;
		//xTar = xt; yTar = yt; zTar = zt;
		//upX = ux; upY = uy; upZ = uz;
		pos = DirectX::XMVectorSet(xPos, yPos, zPos, 1.0f);
		target = DirectX::XMVectorSet(xTar, yTar, zTar, 1.0f);
		up = DirectX::XMVectorSet(upX, upY, upZ, 1.0f);
	}

	void setCamType(CamType ct) { camType = ct; }

	//for iso, ry is the half angle of the cone's tip, rxz is the angle from the x-axis on the xz plane
	//iso assumes the ground level of the scene is on the xz plane and forms a cone from the camera position to that plane
	//the direction the camera looks is down the edge of the cone
	//
	//for fps, ry is the vertical angle from "straight ahead", rxz is the angle from the x-axis on the xz plane
	//
	// x,y,z is the position of the camera in both cases
	void setPosition(float x, float y, float z, float ry, float rxz)
	{
		xPos = x;
		yPos = y;
		zPos = z;
		float radius = 0;

		//std::wstringstream a;

		float radRY = ry * PI / 180;
		float radRXZ = rxz * PI / 180;

		switch (camType)
		{
		case ISO:
			//test breakpoint
//			if (rxz != 0)
//				std::cout << "break";

			radius = y * tan(radRY); //radius of base of cone on xz plane

			xTar = xPos + radius * sin(radRXZ);
			yTar = 0;
			zTar = zPos + radius * cos(radRXZ);
			//a << "ry: " << ry << " ,radRY: " << radRY << " ,rxz: " << rxz << " ,radRXZ: " << radRXZ << "\n";
			//OutputDebugString(a.str().c_str());
			break;
		case FPS:
		default:
			xTar = xPos + cos(ry);
			yTar = yPos + sin(rxz);
			zTar = zPos + sin(ry);
			break;
		}

		pos = DirectX::XMVectorSet(xPos, yPos, zPos, 1.0f);
		target = DirectX::XMVectorSet(xTar, yTar, zTar, 1.0f);
		up = DirectX::XMVectorSet(upX, upY, upZ, 1.0f);
		//graphics->setViewMatrix(pos, target, up);
	}

	//TODO: finish this
	void setPosition(float x, float y, float z, float tx, float ty, float tz, float ux, float uy, float uz)
	{
		xPos = x;
		yPos = y;
		zPos = z;

		pos = DirectX::XMVectorSet(x, y, z, 1.0f);
		target = DirectX::XMVectorSet(tx, ty, tz, 1.0f);
		up = DirectX::XMVectorSet(ux, uy, uz, 1.0f);
		//graphics->setViewMatrix(pos, target, up);
	}

	
	//TODO: quat
	void rotateWorld(float a, float x, float y, float z)
	{
		worldRotation = {

		};
	}

	DirectX::XMVECTOR getPosition()
	{
		return pos;
	}

	DirectX::XMVECTOR getTarget()
	{
		return target;
	}

	DirectX::XMVECTOR getUp()
	{
		return up;
	}

	DirectX::XMMATRIX getWorldRotationMatrix()
	{
		return worldRotation;
	}
};

#endif