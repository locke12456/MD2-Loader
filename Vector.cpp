//#include <Vector Class\Vector.h>
#include "Vector.h"

#define PI 3.141592654


Vector3::Vector3(float x1, float y1, float z1) {
	x = x1;
	y = y1;
	z = z1;
}

Vector3	Vector3::operator-(Vector3 a) 
{
	return Vector3(x - a.x, y - a.y, z - a.z);
}

Vector3	Vector3::operator-(float a)
{
	return Vector3(x - a, y - a, z - a);
}
Vector3	Vector3::operator+(Vector3 a) 
{
	return Vector3(a.x + x, a.y + y, a.z + z);
}

void	Vector3::operator+=(Vector3 a)
{
	x += a.x;
	y += a.y;
	z += a.z;
}

Vector3	Vector3::operator*(Vector3 a)
{
	return	Vector3(x * a.x, y * a.y, z * a.z);
}
Vector3	Vector3::operator/(Vector3 a)
{
	return	Vector3(a.x / x, a.y / y, a.z / z);
}
Vector3	Vector3::operator*(float a) 
{
	return Vector3(x * a, y * a, z * a);
}

Vector3	Vector3::operator/(float a) 
{
	return Vector3(x / a, y / a, z / a);
}
Vector3	Vector3::operator+(float a) 
{
	return Vector3(x + a, y + a, z + a);
}
Vector3	CrossProduct(Vector3 a, Vector3 b) 
{
	Vector3 temp;
	temp.x = (a.y * b.z) - (a.z * b.y);
	temp.y = (a.z * b.x) - (a.x * b.z);
	temp.z = (a.x * b.y) - (a.y * b.x);
	return temp;
}

void	DrawGLVector(Vector3*to, Vector3*from, float red, float green, float blue, int width)
{
	if(width)
		glLineWidth(width);

	glColor3f(red, green, blue);
	glBegin(GL_LINES);
	glVertex3f(from->x, from->y, from->z);
	glVertex3f(to->x, to->y, to->z);
	glEnd();
	glColor3f(1, 1, 1);

	glLineWidth(1); //reset to default
}

float	DotProduct(Vector3 a, Vector3 b) 
{
	float	DP = ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
	return DP;
}

float	DotProduct(Vector3 *a, Vector3 *b) 
{

	float	DP = ((a->x * b->x) + (a->y * b->y) + (a->z * b->z));
	return DP;
}

void	Vector3::Normalize()
{
	float	magnitude = sqrt( (x * x) + (y * y) + (z * z) );
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
}

float	Vector3::GetLength() 
{
	float Length;
	Length = sqrt(x * x + y * y + z * z);
	return Length;
}

Vector2 Vector2::operator-(Vector2 a)
{
	return Vector2(u - a.u, v - a.v);
}

Vector2	Vector2::operator+(Vector2 a)
{
	return Vector2(u + a.u, v + a.v);
}

Vector2	Vector2::operator*(float a) 
{
	return Vector2(u * a, v * a);
}

Vector2 Vector2::operator/(float a) 
{
	return Vector2(u / a, v / a);
}

void	ProjectOntoPlane(Vector3	*NormalToPlane, Vector3	*VecToProject)
{
	Vector3	Projection = Parallel(VecToProject, NormalToPlane); 
	*VecToProject = *VecToProject - Projection;
}

Vector3	Perp(Vector3	*a, Vector3	*axis)
{
	Vector3	temp = *a - Parallel(a, axis);
	return	temp;
}

Vector3	Parallel(Vector3	*a, Vector3	*axis)
{
	Vector3	temp = *axis * DotProduct(a, axis);
	return	temp;
}

Vector3	Mirror(Vector3	*a, Vector3	*axis)
{
	Vector3	temp = *a - (Perp(a, axis)*2);
	return	temp;
}

void	RotateVector(float	Angle, Vector3	*VecToRotAbout, Vector3	*VecToRotate)
{
	float cosTheta = (float)cos(Angle);
	float sinTheta = (float)sin(Angle);
*VecToRotate=Perp(VecToRotate,VecToRotAbout)*cosTheta+(CrossProduct(*VecToRotAbout, *VecToRotate)*sinTheta) 
+ Parallel(VecToRotate, VecToRotAbout); 
}
float	absolute(float a)
{
	if(a < 0)
		return -a;
	return a;
}

float	DEG2RAD(float	a)
{
	return	(a * PI) / 180.0f;
}

float	DegreesToRadians(float Degrees) 
{
	float	Radians = ((2 * PI) * Degrees) / 360;
	return Radians;
}
