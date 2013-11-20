#ifndef	CAMERA_H
#define CAMERA_H


#include "Vector.h"

#define	InAir	1

class Camera
{
public:
	Camera() {}
	Camera(float x, float y, float z);

	void	RotateView(float, Vector3 *);
	bool	CheckKeyboard();
	bool	CheckMouse();
	void	CheckShoot();
	
	//Puts the camera at exactly height units above the ground
	void	SumVelocities();
	
	void	HandleCollisions();
		void	HandleClimb();
		void	HandleSlide();
		void	HandleHeight();
		void	HandleGravity();
		void	PushCamDown();	//Needed during HandleClimb

	float	LookForGround();
	
	void	MoveCamera(Vector3 *to); //Location to move to
	
	Vector3	mFinalVelocity;
	Vector3	mGravityVector;	//just the directional pull of the gravity

	Vector3	mPosition;
	Vector3	mStrafe; //always going to start as crossproduct(view, up)
	Vector3 mUp; //always going to start as (0, 1, 0)
	Vector3 mView; //always going to start as (0, 0, -1)
	Vector3	mKeyboardInput; 
	Vector3 mXAxis;
	Vector3	mYAxis;
	Vector3 mZAxis;

	Vector3 mMoveDir; //the direction player moves in, equal to the view vector projected onto x/z plane

	float	mAirFraction;
	float	mSpeed; //how fast cam moves
	float	xRadians; //how many radians rotated about the x axis, will not go past 1 radian (about 57.3 degrees)
	float	XDegRad;
	float	YDegRad;

	float	TimeInAir;

	unsigned	int	States;
};

#endif
