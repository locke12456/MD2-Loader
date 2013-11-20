#include "Camera.h"
#include "Defines.h"
#include "Quake3BSP.h"
#include "FPS.h"
#define Total 10
extern	FPS_t	FPS;
extern	BSP	bsp;
extern	std::ofstream trace;

#define	INFINITY 10000
#define GROUND_INFINITY 500
extern	Vector3	CollisionVertex[];
extern	Vector3	IncidenceVertex[];
extern	Vector3	ReflectionVertex;
extern	Vector3	CollisionNormal;
extern	Vector3	IntersectPoint;
extern	Vector3	EndIntersectPoint;
extern int newCol;
extern Vector3 rot;
extern MODEL Sp3d;
Vector3	BoxMaxs(30, 30, 30); //not in use for right now 

extern	int		mousedown;

#define	GROUND_EPSILON	1
#define	RADIUS 25
#define DIST_BETWEEN_ENDPOINTS	40
#define HEIGHT  DIST_BETWEEN_ENDPOINTS + RADIUS
#define STEP_UP (HEIGHT/10)
#define	FALLRATE	(HEIGHT*32)/6

#define	assert(x)	x?1:0;

Camera::Camera(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
	
	mView = mPosition + Vector3(0, 0, 1);
	mUp = Vector3(0, 1, 0);
	mStrafe = CrossProduct(mView-mPosition, mUp);
	
	mXAxis = Vector3(1, 0, 0);
	mYAxis = Vector3(0, 1, 0);
	mZAxis = Vector3(0, 0, 1);
	mKeyboardInput = Vector3(0, 0, 0);
	mGravityVector = Vector3(0, -1, 0);
	mAirFraction = .5;
	mSpeed = 100; //number of units per second to travel 
	xRadians = 0; //cam must start facing horizontally !
}

void	Camera::RotateView(float  angle, Vector3 * VecToRotAbout) 
{
	Vector3 NewView;
	
	Vector3 Dir = mView-mPosition;		

	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);
	
	
	//This works the same as Polar Coordinates, except you're on an arbitrary plane, not XY plane or whatever 
NewView=
(Perp(&Dir,VecToRotAbout)*cosTheta)+
(CrossProduct(*VecToRotAbout,Dir)*sinTheta)+Parallel(&Dir,VecToRotAbout);

	mView = (mPosition + NewView);


}

bool	Camera::CheckMouse() 
{
	POINT MousePos;
	GetCursorPos(&MousePos);
	SetCursorPos(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
	if(MousePos.x == MIDDLEX && MousePos.y == MIDDLEY)
		return false; 
	float YDeviation = (MIDDLEY - MousePos.y);		
	float XDeviation = (MIDDLEX - MousePos.x);		
	
	XDegRad = (TWOPI * YDeviation) / SCREEN_WIDTH; //radians to rotate view vector about X axis
	xRadians += XDegRad; 
	if(xRadians > HALFPI)
	{
		xRadians = HALFPI;
		XDegRad = 0;
	}

	if(xRadians < -HALFPI)
	{
		xRadians = -HALFPI;
		XDegRad = 0;
	}

	YDegRad = (TWOPI * XDeviation) / SCREEN_HEIGHT; //radians to rotate view vector about Y axis	
	float cosXTheta = (float)cos(XDegRad);	
	float sinXTheta = (float)sin(XDegRad);	
	
	float cosYTheta = (float)cos(YDegRad);	
	float sinYTheta = (float)sin(YDegRad);	

	//calculate strafe vector
	mStrafe = CrossProduct(mView-mPosition, mYAxis);
	mStrafe.Normalize();

	RotateView(XDegRad, &mStrafe);
	RotateView(YDegRad, &mYAxis);	
	rot.y=rot.y+(YDegRad*180.0f/PI);
	
	//FIXME: does mUp need to be updated?  Seems like it doesn't
//	mUp = CrossProduct(mStrafe, mView-mPosition);
	
	return true;
}

void	Camera::CheckShoot()
{
	if(mousedown)
	{
		mousedown=0;
		memset(&Sp3d,0,sizeof(Sp3d));
		  Sp3d.atk=1;
		  Sp3d.frameRate=15;

		  //shoot/
		//(++newCol)%=Total;
		//Vector3	Target = mView - mPosition;
		//Target.Normalize();
		//CollisionVertex[newCol]=mPosition;
		//Target = mPosition + Target*10000;
		//float	fraction = bsp.Trace(&mPosition, &Target, 0);
		//Vector3	Normal = bsp.mBSPColInfo.hitNormal;
		///*CollisionVertex*/ CollisionNormal = mPosition + ((Target-mPosition) * fraction);
		//Vector3	IncidentDir = mPosition - CollisionVertex[newCol];
		//IncidentDir.Normalize();
		////IncidenceVertex = CollisionVertex + (IncidentDir * 10);
		//
		//Vector3	ReflectDir = Mirror(&IncidentDir, &bsp.mBSPColInfo.hitNormal);
		//ReflectDir.Normalize();
		//IncidenceVertex[newCol]=(CollisionNormal-CollisionVertex[newCol])/10.0f;// = CollisionVertex;// + (ReflectDir * 10);
		
		
		  
		  
		  
		  
		  
		//no use
		//CollisionNormal = CollisionVertex + (Normal*10);
		//Vector3	DirToMove = mView - mPosition;
		//mKeyboardInput = 
		//	States	&	InAir?
		//	mKeyboardInput + (DirToMove * ((FPS.TimeFrac * mSpeed) * mAirFraction) ):
		//	mKeyboardInput + (DirToMove * (FPS.TimeFrac * mSpeed) );
		//float	Fraction = bsp.Trace(&mPosition, &Vector3(mPosition + mKeyboardInput), RADIUS);
		//
		//if(Fraction == 1.0f) //nothing blocked the move so just move to the endpoint and quit your bitching	
		//{	
		//	Vector3	NewPos = mPosition + mKeyboardInput;
		//	MoveCamera(&NewPos);
		//	return;
		//}
		//else //bumped into something along the way 
		//{
		//	//FIXME: does this order matter?
		//	//FIXME: should pass in original fraction to these so they don't have to be re traced?
		////	HandleClimb();	//tries to step up a stair
		//	HandleSlide();
		//}

		
	}
}

bool	Camera::CheckKeyboard()
 {
		bool	KeyboardUpdated = false;
		Vector3	DirToMove = mView - mPosition;
		DirToMove.Normalize(); //may not be necessary!
	if(GetKeyState('W') & 0x80)
	{
		mKeyboardInput = 
			States	&	InAir?
			mKeyboardInput + (DirToMove * ((FPS.TimeFrac * mSpeed) * mAirFraction) ):
			mKeyboardInput + (DirToMove * (FPS.TimeFrac * mSpeed) );
			KeyboardUpdated = true;
	  memset(&Sp3d,0,sizeof(Sp3d));
	  Sp3d.run=1;
      Sp3d.frameRate=7;
			
	}
	if(GetKeyState('S') & 0x80)
	{
		mKeyboardInput =
			States	&	InAir?
			mKeyboardInput - (DirToMove * ((FPS.TimeFrac * mSpeed) * mAirFraction) ):
			mKeyboardInput - (DirToMove * (FPS.TimeFrac * mSpeed) );
		KeyboardUpdated = true;
	  memset(&Sp3d,0,sizeof(Sp3d));
	  Sp3d.run=1;
      Sp3d.frameRate=7;
		
	}
	if(GetKeyState('A') & 0x80)
	{
		mKeyboardInput = 
			States	&	InAir?
			mKeyboardInput - (mStrafe * ((FPS.TimeFrac * mSpeed) * mAirFraction)):
			mKeyboardInput - (mStrafe * (FPS.TimeFrac * mSpeed) );
		KeyboardUpdated = true;
	  memset(&Sp3d,0,sizeof(Sp3d));
	  Sp3d.run=1;
      Sp3d.frameRate=7;
	}
	if(GetKeyState('D') & 0x80)
	{
		mKeyboardInput = 
			States	&	InAir?
			mKeyboardInput + (mStrafe * ((FPS.TimeFrac * mSpeed) * mAirFraction) ):
			mKeyboardInput + (mStrafe * (FPS.TimeFrac * mSpeed));
		KeyboardUpdated = true;
	  memset(&Sp3d,0,sizeof(Sp3d));
	  Sp3d.run=1;
      Sp3d.frameRate=7;
	}
	return	KeyboardUpdated;
}

void	Camera::HandleCollisions()
{
	
float	Length = mKeyboardInput.GetLength();
		if(Length) //prevent a divide by zero, stupid loser 
		{
			ProjectOntoPlane(&mYAxis, &mKeyboardInput);
			mKeyboardInput.x /= Length;
			mKeyboardInput.y /= Length;
			mKeyboardInput.z /= Length;
			mKeyboardInput = mKeyboardInput * Length; //Preserves the original length of the move no matter where you were originally looking
		}

float	Fraction = bsp.Trace(&mPosition, &Vector3(mPosition + mKeyboardInput), RADIUS);
	
	if(Fraction == 1.0f) //nothing blocked the move so just move to the endpoint and quit your bitching	
	{	
		Vector3	NewPos = mPosition + mKeyboardInput;
		MoveCamera(&NewPos);
		return;
	}
	else //bumped into something along the way 
	{
		//FIXME: does this order matter?
		//FIXME: should pass in original fraction to these so they don't have to be re traced?
	//	HandleClimb();	//tries to step up a stair
		HandleSlide();
	}
}


//FIXME: this only tries going up one step
//what if your move is long enough to climb two steps in one frame?
//If going to implement multiple step climbing per frame then SetHeight must be called in this function
#define	REQUIRED_FRACTION_FOR_STAIR .5 //FIXME: magic number as of right now
//Makes sense to trace from mPosition = mPosition + Vector3(0, STEP_UP, 0), so leave that
void	Camera::HandleClimb()
{
	//Traces from current position to a new point just above the player's head
	Vector3	Start, End;	
	float	Fraction;
	End = mPosition + Vector3(0, STEP_UP, 0);
	Fraction = bsp.Trace(&mPosition, &End, RADIUS);
	if(Fraction > 0) //If you could step up without bumping into anything
	{
		//This tries tracing from the new elevated position to the wanted move	
		Start = mPosition + ((End - mPosition) * Fraction); //calculate position above head 
		End = Start + mKeyboardInput;
		Fraction = bsp.Trace(&Start, &End, RADIUS);
		if(Fraction > REQUIRED_FRACTION_FOR_STAIR) //we could trace from elevated position forward
		{
			Vector3	Intersect = Start + ((End - Start) * Fraction);
			MoveCamera	(&Intersect);
			PushCamDown();	//FIXME: this really is just a hack for now but it works all right
			return;
		}
		else
			return;
	}
	else
		return;
}

void	Camera::PushCamDown()
{
	Vector3	End = mPosition - Vector3(0, GROUND_INFINITY, 0);
	float	Fraction = bsp.Trace(&mPosition, &End, RADIUS);
	Vector3	EndIntersect = mPosition + ((End - mPosition) * Fraction);
	
	Vector3	NewPos = EndIntersect + Vector3(0, DIST_BETWEEN_ENDPOINTS, 0);
	States	&=	(InAir&0); 
	MoveCamera(&NewPos);
	return;
}

//FIXME: probably want to pass a float into this function instead of re tracing

void	Camera::HandleSlide()
{
	Vector3	IntersectPoint, InBrush;
	float	Fraction = bsp.Trace(&mPosition, &Vector3(mPosition+mKeyboardInput), RADIUS);
	IntersectPoint	=	mPosition + (mKeyboardInput*Fraction);
	InBrush	=	(mPosition+mKeyboardInput) - IntersectPoint;
	
	while(1)
	{
		if(absolute(InBrush.x) <= .0001f && absolute(InBrush.y) <= .0001 && absolute(InBrush.z) <= .0001 )
		{
			return;
		}
		else
		{
			ProjectOntoPlane(&bsp.mBSPColInfo.hitNormal, &InBrush);
			Fraction	=	bsp.Trace(&mPosition, &Vector3(mPosition + InBrush), RADIUS);
			if(Fraction >= 1.0f)
			{
				Vector3	NewPos = mPosition + (InBrush*Fraction);
				MoveCamera(&NewPos);
				return;
			}

		}
	}
}

void	Camera::HandleHeight()
{
	Vector3	End, EndIntersect;
	Vector3	Intersect, InfiniteRay;
	float	IntersectFrac;
	float	EndFrac;
	int yes = 0;

	End	=	mPosition - Vector3(0, GROUND_INFINITY, 0);
	EndFrac	=	bsp.Trace(&mPosition, &End, RADIUS);
	EndIntersect = mPosition + ((End-mPosition) * EndFrac);
	EndIntersectPoint = EndIntersect;
	
	if(mPosition.y - EndIntersect.y > DIST_BETWEEN_ENDPOINTS)
	{
			States	|=	(InAir&1);
			TimeInAir	+=	(FPS.TimeFrac);
			Vector3	NewPos = mPosition + (Vector3(0, -1, 0) * (15 * TimeInAir)); //hack until I find an appropriate
			float	Fraction	=	bsp.Trace(&mPosition, &NewPos, RADIUS);      //conversion from 'units' to feet or meters
			NewPos = mPosition + ((NewPos - mPosition) * Fraction);
				MoveCamera(&NewPos);
			return;
	}
	else	if(mPosition.y - EndIntersect.y < DIST_BETWEEN_ENDPOINTS)
	{
			TimeInAir = 0;
			States	&=	(InAir&0);
			Vector3	NewPos	=	EndIntersect + Vector3(0, DIST_BETWEEN_ENDPOINTS, 0);
			float	Fraction	=	bsp.Trace(&mPosition, &NewPos, RADIUS);
			NewPos	=	mPosition + ((NewPos-mPosition) * Fraction);
				MoveCamera(&NewPos);
			return;
	}
}

void	Camera::MoveCamera(Vector3	*to)
{
	Vector3	Delta = *to - mPosition;
	mView = mView + Delta;
	mPosition = *to;
}

