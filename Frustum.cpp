#include "Frustum.h"
#include "Quake3BSP.h" //for tVector3i
#include "camera.h"

extern	Camera	cam;
extern	std::ofstream	trace;

void	Frustum::GetClipPlanes()
{
	float   PROJECTION_MATRIX[16];							
	float   MODELVIEW_MATRIX[16];							
	float   CLIP_PLANES[16];							

	glGetFloatv( GL_MODELVIEW_MATRIX, MODELVIEW_MATRIX );
	glGetFloatv( GL_PROJECTION_MATRIX, PROJECTION_MATRIX );
	
CLIP_PLANES[0]=MODELVIEW_MATRIX[0]*PROJECTION_MATRIX[0]+MODELVIEW_MATRIX[1]*PROJECTION_MATRIX[4]+MODELVIEW_MATRIX[2]*PROJECTION_MATRIX[8]+MODELVIEW_MATRIX[3]*PROJECTION_MATRIX[12];
CLIP_PLANES[1]=MODELVIEW_MATRIX[0]*PROJECTION_MATRIX[1]+MODELVIEW_MATRIX[1]*PROJECTION_MATRIX[5]+MODELVIEW_MATRIX[2]*PROJECTION_MATRIX[9]+MODELVIEW_MATRIX[3]*PROJECTION_MATRIX[13];
CLIP_PLANES[3]=MODELVIEW_MATRIX[0]*PROJECTION_MATRIX[3]+MODELVIEW_MATRIX[1]*PROJECTION_MATRIX[7]+MODELVIEW_MATRIX[2]*PROJECTION_MATRIX[11]+MODELVIEW_MATRIX[3]*PROJECTION_MATRIX[15];
CLIP_PLANES[4]=MODELVIEW_MATRIX[4]*PROJECTION_MATRIX[0]+MODELVIEW_MATRIX[5]*PROJECTION_MATRIX[4]+MODELVIEW_MATRIX[6]*PROJECTION_MATRIX[8]+MODELVIEW_MATRIX[7]*PROJECTION_MATRIX[12];	
CLIP_PLANES[5]=MODELVIEW_MATRIX[4]*PROJECTION_MATRIX[1]+MODELVIEW_MATRIX[5]*PROJECTION_MATRIX[5]+MODELVIEW_MATRIX[6]*PROJECTION_MATRIX[9]+MODELVIEW_MATRIX[7]*PROJECTION_MATRIX[13];
CLIP_PLANES[7]=MODELVIEW_MATRIX[4]*PROJECTION_MATRIX[3]+MODELVIEW_MATRIX[5]*PROJECTION_MATRIX[7]+MODELVIEW_MATRIX[6]*PROJECTION_MATRIX[11]+MODELVIEW_MATRIX[7]*PROJECTION_MATRIX[15];
CLIP_PLANES[8]=MODELVIEW_MATRIX[8]*PROJECTION_MATRIX[0]+MODELVIEW_MATRIX[9]*PROJECTION_MATRIX[4]+MODELVIEW_MATRIX[10]*PROJECTION_MATRIX[8]+MODELVIEW_MATRIX[11]*PROJECTION_MATRIX[12];
CLIP_PLANES[9]=MODELVIEW_MATRIX[8]*PROJECTION_MATRIX[1]+MODELVIEW_MATRIX[9]*PROJECTION_MATRIX[5]+MODELVIEW_MATRIX[10]*PROJECTION_MATRIX[9]+MODELVIEW_MATRIX[11]*PROJECTION_MATRIX[13];
CLIP_PLANES[11]=MODELVIEW_MATRIX[8]*PROJECTION_MATRIX[3]+MODELVIEW_MATRIX[9]*PROJECTION_MATRIX[7]+MODELVIEW_MATRIX[10]*PROJECTION_MATRIX[11]+MODELVIEW_MATRIX[11]*PROJECTION_MATRIX[15];

	mFrustum[RIGHT].Normal.x = CLIP_PLANES[ 3] - CLIP_PLANES[ 0];
	mFrustum[RIGHT].Normal.y = CLIP_PLANES[ 7] - CLIP_PLANES[ 4];
	mFrustum[RIGHT].Normal.z = CLIP_PLANES[11] - CLIP_PLANES[ 8];
	mFrustum[RIGHT].Dist = DotProduct(&mFrustum[RIGHT].Normal, &cam.mPosition); //WORKS! Keep before NormalizeFrustum

	mFrustum[BOTTOM].Normal.x = CLIP_PLANES[3] + CLIP_PLANES[1];
	mFrustum[BOTTOM].Normal.y = CLIP_PLANES[7] + CLIP_PLANES[5];
	mFrustum[BOTTOM].Normal.z = CLIP_PLANES[11] + CLIP_PLANES[9];
	mFrustum[BOTTOM].Dist = DotProduct(&mFrustum[BOTTOM].Normal, &cam.mPosition); //WORKS
	
		float	magnitude;
		magnitude = mFrustum[BOTTOM].Normal.GetLength();
		mFrustum[BOTTOM].Dist /= magnitude;
		mFrustum[BOTTOM].Normal.x /= magnitude;
		mFrustum[BOTTOM].Normal.y /= magnitude;
		mFrustum[BOTTOM].Normal.z /= magnitude;

		magnitude = mFrustum[RIGHT].Normal.GetLength();
		mFrustum[RIGHT].Dist /= magnitude;
		mFrustum[RIGHT].Normal.x /= magnitude;
		mFrustum[RIGHT].Normal.y /= magnitude;
		mFrustum[RIGHT].Normal.z /= magnitude;
	

	//FIXME: close clip plane's distance inaccurate, may be a problem?
	Vector3	view = cam.mView - cam.mPosition;
	view.Normalize();
	Vector3	PointOnPlane;

	mFrustum[LEFT].Normal = Mirror(&mFrustum[RIGHT].Normal, &view);
	mFrustum[LEFT].Dist = DotProduct(&mFrustum[LEFT].Normal, &cam.mPosition);

	mFrustum[TOP].Normal = Mirror(&mFrustum[BOTTOM].Normal, &view);
	mFrustum[TOP].Dist = DotProduct(&mFrustum[TOP].Normal, &cam.mPosition);
	
	mFrustum[FAR].Normal = view * -1; 
	PointOnPlane = cam.mPosition + ((cam.mView-cam.mPosition) * FAR_CLIP_PLANE);
	mFrustum[FAR].Dist = DotProduct(&mFrustum[FAR].Normal, &PointOnPlane);

	mFrustum[CLOSE].Normal = view;
	PointOnPlane = cam.mPosition + ((cam.mView-cam.mPosition) * CLOSE_CLIP_PLANE);
	mFrustum[CLOSE].Dist = DotProduct(&mFrustum[CLOSE].Normal, &cam.mPosition);
}

void	Frustum::NormalizeFrustum()
{
//Loop through every side of the frustum
	//get the length
	//divide d by length
	//normalize normal
	float	magnitude;
	for(int i = 0; i < 6; i++)
	{
		magnitude = mFrustum[i].Normal.GetLength();
		mFrustum[i].Dist /= magnitude;
		mFrustum[i].Normal.x /= magnitude;
		mFrustum[i].Normal.y /= magnitude;
		mFrustum[i].Normal.z /= magnitude;
	}
}


//All points should be behind a plane to return false
int		Frustum::BBInFrustum(tVector3i	mins, tVector3i	maxs)
{
	for(int i = 0; i < 6; i++)
	{
	if(!BehindPlane(mins.x,mins.y, mins.z, i)) continue;
	if(!BehindPlane(maxs.x, mins.y, mins.z, i)) continue;
	if(!BehindPlane(mins.x, maxs.y, mins.z, i)) continue;
	if(!BehindPlane(maxs.x, maxs.y, mins.z, i)) continue;
	if(!BehindPlane(mins.x, mins.y, maxs.z, i)) continue;
	if(!BehindPlane(maxs.x, mins.y, maxs.z, i)) continue;
	if(!BehindPlane(mins.x, maxs.y, maxs.z, i)) continue;
	if(!BehindPlane(maxs.x, maxs.y, maxs.z, i)) continue;
	return false; //box is completely behind a single frustum plane and should not be drawn
	}

	return true; //enough (or all) of the box is in frustum and should be drawn
}

bool	Frustum::BehindPlane(int x, int y, int z, int i)
{
if((mFrustum[i].Normal.x * x + mFrustum[i].Normal.y * y + mFrustum[i].Normal.z * z - mFrustum[i].Dist) >= 0)
	return false;
	return true;
}


//Loop through each side of the frustum
//If the point is behind ANY of the frustum planes then it isn't visible at all
//FIXME: this part can probably be optimized even more 
bool	Frustum::PointInFrustum(float x, float	y, float z)
{
	for(int i = 0; i < 6; i++)
	{
		if((mFrustum[i].Normal.x * x + mFrustum[i].Normal.y * y + mFrustum[i].Normal.z * z - mFrustum[i].Dist) >= 0)
			return true; 
	}
	return false;
}
