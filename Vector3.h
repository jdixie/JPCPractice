#ifndef _VECTOR3_H
#define _VECTOR3_H
#define WIN32_LEAN_AND_MEAN

class Vector3
{
public:
	float x, y, z;

	Vector3()
	{
		x = y = z = 0;
	}

	Vector3(float a, float b, float c)
	{
		x = a; y = b; z = c;
	}

	Vector3(Vector3 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	Vector3 plus(Vector3 v)
	{
		Vector3 r(x + v.x, y + v.y, z + v.z);
		return r;
	}

	Vector3 minus(Vector3 v)
	{
		Vector3 r(x - v.x, y - v.y, z - v.z);
		return r;
	}

	Vector3 mult(float f) 
	{ 
		Vector3 r(f * x, f * y, f * z);
		return r; 
	}

	Vector3 cross(Vector3 v)
	{
		Vector3 r(y * v.z - v.y * z,
			z * v.x - v.z * x,
			x * v.y - v.x * y);
		return r;
	}

	float dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	float length()
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	Vector3 normal()
	{
		float l = length();
		Vector3 r(x / l, y / l, z / l);
		return r;
	}

	//radians
	float radiansBetween(Vector3 v)
	{
		return std::acos(dot(v) / (length() * v.length()));
	}

	//degrees
	float angleBetween(Vector3 v)
	{
		return std::acos(dot(v) / (length() * v.length())) * 180 / PI;
	}
};

#endif