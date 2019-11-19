#ifndef POINT3D_H
#define POINT3D_H

#include "math.h"
#include <Utilities/Pnt3f.H>

#include <QtCore/qglobal.h>

struct Point3d
{
	float x, y, z;

	Point3d()
		: x(0)
		, y(0)
		, z(0)
	{
	}

	Point3d(Pnt3f p):x(p.x), y(p.y), z(p.z){}

	Point3d(float x_, float y_, float z_)
		: x(x_)
		, y(y_)
		, z(z_)
	{
	}

	Point3d operator+(const Point3d &p) const
	{
		return Point3d(*this) += p;
	}

	Point3d operator-(const Point3d &p) const
	{
		return Point3d(*this) -= p;
	}

	Point3d operator*(float f) const
	{
		return Point3d(*this) *= f;
	}
	Point3d &operator+=(const Point3d &p)
	{
		x += p.x;
		y += p.y;
		z += p.z;
		return *this;
	}

	Point3d &operator-=(const Point3d &p)
	{
		x -= p.x;
		y -= p.y;
		z -= p.z;
		return *this;
	}

	bool operator==(const Point3d &p)
	{
		if((x==p.x)&&(y==p.y)&&(z==p.z))
			return true;
		return false;
	}

	Point3d &operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	Point3d normalize() const
	{
		float r = 1. / sqrt(x * x + y * y + z * z);
		return Point3d(x * r, y * r, z * r);
	}
	float &operator[](unsigned int index) {
		Q_ASSERT(index < 3);
		return (&x)[index];
	}

	const float &operator[](unsigned int index) const {
		Q_ASSERT(index < 3);
		return (&x)[index];
	}

	Point3d Point3d::matmul(float m[3][3]) {
		float tx = m[0][0] * x + m[0][1] * y + m[0][2] * z,
			ty = m[1][0] * x + m[1][1] * y + m[1][2] * z,
			tz = m[2][0] * x + m[2][1] * y + m[2][2] * z;
		return Point3d(tx, ty, tz);
	}
};

inline float dot(const Point3d &a, const Point3d &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Point3d cross(const Point3d &a, const Point3d &b)
{
	return Point3d(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

#endif