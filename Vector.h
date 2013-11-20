#ifndef VECTOR_H
#define VECTOR_H

#include "Defines.h"

class Vector3 
{
public:
	Vector3(){}
	Vector3(float x1, float y1, float z1);
	
	Vector3	operator-(Vector3);	//CAN ONLY PASS IN ONE PARAMETER TO OVERLOADED OPERATOR
	Vector3	operator-(float);
	Vector3	operator+(Vector3);
	Vector3	operator+(float);
	Vector3	operator/(float);
	Vector3	operator/(Vector3);
	Vector3 operator*(float);
	Vector3	operator*(Vector3);

	void	operator+=(Vector3);


	float x;
	float y;
	float z;

	void	Normalize(); 
	float	GetLength();
};

struct Vector2 
{
	Vector2(){}
	Vector2(float x1, float y1) {u = x1; v = y1;}
	
	Vector2	operator-(Vector2);
	Vector2	operator+(Vector2);
	Vector2 operator/(float);
	Vector2 operator*(float);
	float u; //x
	float v; //y
};

Vector3	CrossProduct(Vector3 a, Vector3 b);

float	DotProduct(Vector3, Vector3);
float	DotProduct(Vector3*, Vector3*);

//Vector3	ProjectOntoPlane(Vector3 &, Vector3 &, Vector3 &);
void	ProjectOntoPlane(Vector3	*, Vector3	*);
void	RotateVector(float Angle, Vector3	*VecToRotAbout, Vector3	*VecToRotate);

float	absolute(float);

Vector3	Perp(Vector3	*, Vector3	*axis);
Vector3	Parallel(Vector3*, Vector3	*axis);
Vector3	Mirror(Vector3	*, Vector3	*axis);


void	DrawGLVector(Vector3*to, Vector3*from, float red, float green, float blue, int width);

float	DEG2RAD(float);
float	DegreesToRadians(float Degrees) ;
#endif

