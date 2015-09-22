#ifndef _QUATERNION_H
#define _QUATERNION_H
#define WIN32_LEAN_AND_MEAN

class Quaternion
{
public:
	float s, x, y, z;

	Quaternion()
	{
		s = x = y = z = 0;
	}

	Quaternion(float scalar, float vectorX, float vectorY, float vectorZ)
	{
		s = scalar;
		x = vectorX;
		y = vectorY;
		z = vectorZ;
	}

	//this quaternion times another: this * q
	Quaternion quatMult(Quaternion q)
	{
		float newScalar = s * q.s - x * q.x - y * q.y - z * q.z;
		float newX = s * q.x + x * q.s + y * q.z - z * q.y;
		float newY = s * q.y + y * q.s + z * q.x - x * q.z;
		float newZ = s * q.z + z * q.s + x * q.y - y * q.x;
		Quaternion n(newScalar, newX, newY, newZ);
		return n;
	}

	Quaternion getConjugate()
	{
		Quaternion n(s, -x, -y, -z);
		return n;
	}

	float getNorm()
	{
		return (float)std::sqrt(s*s + x*x + y*y + z*z);
	}

	Quaternion getNormalized()
	{
		float n = getNorm();
		Quaternion r(s / n, x / n, y / n, z / n);
		return r;
	}

	void resetQuat()
	{
		s = 1;
		x = 0;
		y = 0;
		z = 0;
	}

};

#endif