#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Quake3BSP.h"
#include "Vector.h"

#define RIGHT 0
#define LEFT 1
#define BOTTOM 2
#define TOP 3
#define CLOSE 4 //originally BACK
#define FAR 5 //originally FRONT

typedef	struct 
{
	Vector3	Normal;
	float	Dist;
} Plane_t;


class Frustum
{
public:
	Frustum()
	{
	}
	
	void	UpdateFrustum();
		void	GetClipPlanes(); 
		void	NormalizeFrustum();
		
	//contains the boxes minimum and maximum points (diagonals)
	int		BBInFrustum(tVector3i	mins, tVector3i	maxs);
	bool	PointInFrustum(float x, float y, float z);
	bool	BehindPlane(int x, int y, int z, int plane); 

	Plane_t	mFrustum[6];
};

#endif
