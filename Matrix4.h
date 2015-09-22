#ifndef _MATRIX4_H
#define _MATRIX4_H

class Matrix4
{
public:
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;

//public:
	Matrix4()
	{
		setIdentity();
	}

	void setIdentity()
	{
		m00 = m11 = m22 = m33 = 1;
		m01 = m02 = m03 = 0;
		m10 = m12 = m13 = 0;
		m20 = m21 = m23 = 0;
		m30 = m31 = m32 = 0;
	}

	Matrix4 multiply(Matrix4 mat)
	{
		Matrix4 temp;

		temp.m00 = m00 * mat.m00 + m01 * mat.m10 + m02 * mat.m20 + m03 * mat.m30;
		temp.m10 = m10 * mat.m00 + m11 * mat.m10 + m12 * mat.m20 + m13 * mat.m30;
		temp.m20 = m20 * mat.m00 + m21 * mat.m10 + m22 * mat.m20 + m23 * mat.m30;
		temp.m30 = m30 * mat.m00 + m31 * mat.m10 + m32 * mat.m20 + m33 * mat.m30;

		temp.m01 = m00 * mat.m01 + m01 * mat.m11 + m02 * mat.m21 + m03 * mat.m31;
		temp.m11 = m10 * mat.m01 + m11 * mat.m11 + m12 * mat.m21 + m13 * mat.m31;
		temp.m21 = m20 * mat.m01 + m21 * mat.m11 + m22 * mat.m21 + m23 * mat.m31;
		temp.m31 = m30 * mat.m01 + m31 * mat.m11 + m32 * mat.m21 + m33 * mat.m31;

		temp.m02 = m00 * mat.m02 + m01 * mat.m12 + m02 * mat.m22 + m03 * mat.m32;
		temp.m12 = m10 * mat.m02 + m11 * mat.m12 + m12 * mat.m22 + m13 * mat.m32;
		temp.m22 = m20 * mat.m02 + m21 * mat.m12 + m22 * mat.m22 + m23 * mat.m32;
		temp.m32 = m30 * mat.m02 + m31 * mat.m12 + m32 * mat.m22 + m33 * mat.m32;

		temp.m03 = m00 * mat.m03 + m01 * mat.m13 + m02 * mat.m23 + m03 * mat.m33;
		temp.m13 = m10 * mat.m03 + m11 * mat.m13 + m12 * mat.m23 + m13 * mat.m33;
		temp.m23 = m20 * mat.m03 + m21 * mat.m13 + m22 * mat.m23 + m23 * mat.m33;
		temp.m33 = m30 * mat.m03 + m31 * mat.m13 + m32 * mat.m23 + m33 * mat.m33;

		return temp;
	}

	Matrix4 multiply(float f)
	{
		Matrix4 temp;

		temp.m00 = f * m00;     temp.m01 = f * m01;     temp.m02 = f * m02;     temp.m03 = f * m03;
		temp.m10 = f * m10;     temp.m11 = f * m11;     temp.m12 = f * m12;     temp.m13 = f * m13;
		temp.m20 = f * m20;     temp.m21 = f * m21;     temp.m22 = f * m22;     temp.m23 = f * m23;
		temp.m30 = f * m30;     temp.m31 = f * m31;     temp.m32 = f * m32;     temp.m33 = f * m33;

		return temp;
	}

	Matrix4 getTranspose()
	{
		Matrix4 temp;

		temp.m00 = m00;
		temp.m10 = m01;
		temp.m20 = m02;
		temp.m30 = m03;

		temp.m01 = m10;
		temp.m11 = m11;
		temp.m21 = m12;
		temp.m31 = m13;

		temp.m02 = m20;
		temp.m12 = m21;
		temp.m22 = m22;
		temp.m32 = m23;

		temp.m03 = m30;
		temp.m13 = m31;
		temp.m23 = m32;
		temp.m33 = m33;

		return temp;
	}

	//this part pretty much taken from GLU and adapted
	Matrix4 getInverse()
	{
		Matrix4 temp;
		double det;
		int i;

		temp.m00 = m11  * m22 * m33 -
			m11  * m23 * m32 -
			m21  * m12  * m33 +
			m21  * m13  * m32 +
			m31 * m12  * m23 -
			m31 * m13  * m22;

		temp.m10 = -m10  * m22 * m33 +
			m10  * m23 * m32 +
			m20  * m12  * m33 -
			m20  * m13  * m32 -
			m30 * m12  * m23 +
			m30 * m13  * m22;

		temp.m20 = m10  * m21 * m33 -
			m10  * m23 * m31 -
			m20  * m11 * m33 +
			m20  * m13 * m31 +
			m30 * m11 * m23 -
			m30 * m13 * m21;

		temp.m30 = -m10  * m21 * m32 +
			m10  * m22 * m31 +
			m20  * m11 * m32 -
			m20  * m12 * m31 -
			m30 * m11 * m22 +
			m30 * m12 * m21;

		temp.m01 = -m01  * m22 * m33 +
			m01  * m23 * m32 +
			m21  * m02 * m33 -
			m21  * m03 * m32 -
			m31 * m02 * m23 +
			m31 * m03 * m22;

		temp.m11 = m00  * m22 * m33 -
			m00  * m23 * m32 -
			m20  * m02 * m33 +
			m20  * m03 * m32 +
			m30 * m02 * m23 -
			m30 * m03 * m22;

		temp.m21 = -m00  * m21 * m33 +
			m00  * m23 * m31 +
			m20  * m01 * m33 -
			m20  * m03 * m31 -
			m30 * m01 * m23 +
			m30 * m03 * m21;

		temp.m31 = m00  * m21 * m32 -
			m00  * m22 * m31 -
			m20  * m01 * m32 +
			m20  * m02 * m31 +
			m30 * m01 * m22 -
			m30 * m02 * m21;

		temp.m02 = m01  * m12 * m33 -
			m01  * m13 * m32 -
			m11  * m02 * m33 +
			m11  * m03 * m32 +
			m31 * m02 * m13 -
			m31 * m03 * m12;

		temp.m12 = -m00  * m12 * m33 +
			m00  * m13 * m32 +
			m10  * m02 * m33 -
			m10  * m03 * m32 -
			m30 * m02 * m13 +
			m30 * m03 * m12;

		temp.m22 = m00  * m11 * m33 -
			m00  * m13 * m31 -
			m10  * m01 * m33 +
			m10  * m03 * m31 +
			m30 * m01 * m13 -
			m30 * m03 * m11;

		temp.m32 = -m00  * m11 * m32 +
			m00  * m12 * m31 +
			m10  * m01 * m32 -
			m10  * m02 * m31 -
			m30 * m01 * m12 +
			m30 * m02 * m11;

		temp.m03 = -m01 * m12 * m23 +
			m01 * m13 * m22 +
			m11 * m02 * m23 -
			m11 * m03 * m22 -
			m21 * m02 * m13 +
			m21 * m03 * m12;

		temp.m13 = m00 * m12 * m23 -
			m00 * m13 * m22 -
			m10 * m02 * m23 +
			m10 * m03 * m22 +
			m20 * m02 * m13 -
			m20 * m03 * m12;

		temp.m23 = -m00 * m11 * m23 +
			m00 * m13 * m21 +
			m10 * m01 * m23 -
			m10 * m03 * m21 -
			m20 * m01 * m13 +
			m20 * m03 * m11;

		temp.m33 = m00 * m11 * m22 -
			m00 * m12 * m21 -
			m10 * m01 * m22 +
			m10 * m02 * m21 +
			m20 * m01 * m12 -
			m20 * m02 * m11;

		det = m00 * temp.m00 + m01 * temp.m10 + m02 * temp.m20 + m03 * temp.m30;

		return temp.multiply(1 / det);
	}

	void makeTranslate(float x, float y, float z)
	{
		m03 = x;
		m13 = y;
		m23 = z;
		m00 = m11 = m22 = m33 = 1;
		m01 = m02 = m10 = m12 = m20 = m21 = m30 = m31 = m32 = 0;
	}

	void makeRotate(float a, float x, float y, float z)
	{
		float rad = a * PI / 180;
		float c = cos(rad);
		float s = sin(rad);

		m00 = x * x * (1 - c) + c;      m01 = x * y * (1 - c) - z * s;  m02 = x * z * (1 - c) + y * s;  m03 = 0;
		m10 = y * x * (1 - c) + z * s;  m11 = y * y * (1 - c) + c;      m12 = y * z * (1 - c) - x * s;  m13 = 0;
		m20 = z * x * (1 - c) - y * s;  m21 = z * y * (1 - c) + x * s;  m22 = z * z * (1 - c) + c;      m23 = 0;
		m30 = 0;                        m31 = 0;                        m32 = 0;                        m33 = 1;
	}

	void makeRotate(Quaternion q)
	{
		float x2 = q.x * q.x;
		float y2 = q.y * q.y;
		float z2 = q.z * q.z;
		float sx = q.s * q.x;
		float sy = q.s * q.y;
		float sz = q.s * q.z;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float yz = q.y * q.z;

		m00 = 1 - 2 * y2 - 2 * z2;      m01 = 2 * xy - 2 * sz;          m02 = 2 * xz + 2 * sy;          m03 = 0;
		m10 = 2 * xy + 2 * sz;          m11 = 1 - 2 * x2 - 2 * z2;      m12 = 2 * yz - 2 * sx;          m13 = 0;
		m20 = 2 * xz - 2 * sy;          m21 = 2 * yz + 2 * sx;          m22 = 1 - 2 * x2 - 2 * y2;      m23 = 0;
		m30 = 0;                        m31 = 0;                        m32 = 0;                        m33 = 1;
	}

	void makeScale(float x, float y, float z)
	{
		m00 = x;
		m11 = y;
		m22 = z;
		m33 = 1;
		m01 = m02 = m03 = 0;
		m10 = m12 = m13 = 0;
		m20 = m21 = m23 = 0;
		m30 = m31 = m32 = 0;
	}

	/*void makeView(Vector3 forward, Vector3 right, Vector3 up, Vector3 eye)
	{
		m00 = right.x;      m01 = right.y;      m02 = right.z;      m03 = -right.dot(eye);
		m10 = up.x;         m11 = up.y;         m12 = up.z;         m13 = -up.dot(eye);
		m20 = -forward.x;   m21 = -forward.y;   m22 = -forward.z;   m23 = forward.dot(eye);
		m30 = 0f;           m31 = 0f;           m32 = 0f;           m33 = 1f;
	}

	void makeOrthographic(float left, float right, float bottom, float top, float near, float far)
	{
		m00 = 2f / (right - left);  m01 = 0f;                   m02 = 0f;                   m03 = -(right + left) / (right - left);
		m10 = 0f;                   m11 = 2f / (top - bottom);  m12 = 0f;                   m13 = -(top + bottom) / (top - bottom);
		m20 = 0f;                   m21 = 0f;                   m22 = -2f / (far - near);   m23 = -(far + near) / (far - near);
		m30 = 0f;                   m31 = 0f;                   m32 = 0f;                   m33 = 1f;
	}

	void makePerspective(float fov, float aspect, float near, float far)
	{
		float y = (float)(1.0f / Math.tan(Math.toRadians(fov) / 2.0f)); //Math.PI / 180.0 for degree to radian calc
		float x = y / aspect;

		m00 = x;                m01 = 0f;               m02 = 0f;                           m03 = 0f;
		m10 = 0f;               m11 = y;                m12 = 0f;                           m13 = 0f;
		m20 = 0f;               m21 = 0f;               m22 = (far + near) / (near - far);  m23 = (2f * far * near) / (near - far);
		m30 = 0f;               m31 = 0f;               m32 = -1f;                          m33 = 0f;
	}

	//TODO: this one isn't working right, recheck formulas
	void makePerspective(float left, float right, float bottom, float top, float near, float far)
	{
		m00 = 2f * near / (right - left);   m01 = 0f;                           m02 = (right + left) / (right - left);       m03 = 0f;
		m10 = 0f;                           m11 = 2f * near / (top - bottom);   m12 = (top + bottom) / (top - bottom);      m13 = 0f;
		m20 = 0f;                           m21 = 0f;                           m22 = (far + near) / (far - near);          m23 = -2f * (far * near) / (far - near);
		m30 = 0f;                           m31 = 0f;                           m32 = -1f;                                  m33 = 0f;
	}*/

	/*public FloatBuffer asBuffer()
	{
		ByteBuffer buffer = ByteBuffer.allocateDirect(64);
		buffer.order(ByteOrder.nativeOrder());
		FloatBuffer mb = buffer.asFloatBuffer();
		mb.clear();

		mb.put(m00); mb.put(m01); mb.put(m02); mb.put(m03);
		mb.put(m10); mb.put(m11); mb.put(m12); mb.put(m13);
		mb.put(m20); mb.put(m21); mb.put(m22); mb.put(m23);
		mb.put(m30); mb.put(m31); mb.put(m32); mb.put(m33);

		mb.flip();

		return mb;
	}*/
};

#endif