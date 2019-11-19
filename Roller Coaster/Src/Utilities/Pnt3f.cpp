/************************************************************************
     File:        Pnt3f.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						simple 3D point class

						Note: for the longest time, we avoided giving out 
						basic data
						structure classes. Everyone likes to make their 
						own, and everyone
						makes different choices in how to use them.

						This one is really basic. It is evolved from the 
						old "Pnt3"
						class that Mike wrote in grad school (circa 1993). 
						It is hard coded for floats (since we didn't want 
						to get started
						with templates). 

						We've only put in a few functions. You can add more 
						if you want.

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include "Utilities/Pnt3f.H"
#include "math.h"
//************************************************************************
//
// *
//========================================================================
Pnt3f::
Pnt3f() : x(0), y(0), z(0)
//=============================================================================
{
}
//*****************************************************************************
//
// *
//=============================================================================
Pnt3f::
Pnt3f(const float* iv) : x(iv[0]), y(iv[1]), z(iv[2])
//=============================================================================
{
}
//*****************************************************************************
//
// *
//=============================================================================
Pnt3f::
Pnt3f(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z)
//=============================================================================
{
}

//*****************************************************************************
//
// *
//=============================================================================
Pnt3f Pnt3f::
normalize()
//=============================================================================
{
	float l = x*x + y*y + z*z;
	if (l<.000001) {
		x = 0;
		y = 1;
		z = 0;
	}
	else {
		l = sqrt(l);
		x /= l;
		y /= l;
		z /= l;
	}
	return *this;
}

void Pnt3f::printself(){
	printf("%f %f %f\n", x, y, z);
}

float Pnt3f::len() {
	return sqrt(x*x + y * y + z * z);
}

void Pnt3f::matmul(float m[3][3]) {
	float tx = m[0][0] * x + m[0][1] * y + m[0][2] * z,
		ty = m[1][0] * x + m[1][1] * y + m[1][2] * z,
		tz = m[2][0] * x + m[2][1] * y + m[2][2] * z;
	x = tx, y = ty, z = ty;
}