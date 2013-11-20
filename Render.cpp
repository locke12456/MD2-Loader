#pragma comment(lib, "glew32.lib")
#define GLEW_STATIC 

#include	"Render.h"
#include	"Objects.h"
#include "Defines.h"
#include <GL/glut.h>
#include "Md2Player.h"
#include "Md3.h"
#define MODEL_PATH  "lara"				// This stores the folder name the character is in
#define MODEL_NAME	"lara"				// This stores the prefix for all the character files
#define GUN_NAME    "Railgun"			// This stores the name of our gun to load
#define Total 10
static void display ();

CModelMD3 *g_Model;						// This is our global md3 character model instance
MODEL Sp3d;
int x,y;
// Timer
struct glut_timer_t
{
  double current_time;
  double last_time;

} timer;

// Camera
//struct Vector3d
//{
//  float x, y, z;
//
//} rot, eye;
Vector3 rot(0,0,0);
Md2Player *player;
Md2Player *emty;
bool bTextured = true;
bool bLightGL = true;
bool bAnimated = true;

int verbose = 2;
int renderMode = 0;
//int Sp3d.frameRate = 7;
int fps = 0;

vector<string> skinList;
vector<string> animList;

//FIXME: clean up all of this shit someday
	FPS_t FPS;
	extern	std::ofstream	trace;
	BSP	bsp;
	Camera cam(250, 100, -10);
	Frustum	frus;
PFNGLMULTITEXCOORD2FARBPROC	glMultiTexCoord2fARB	= NULL;
PFNGLACTIVETEXTUREARBPROC	glActiveTextureARB	= NULL;
typedef BOOL (WINAPI*SwapProc)(int);
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

Vector3	CollisionVertex[Total];
Vector3	IncidenceVertex[Total];
Vector3 ReflectionVertex(0, 0, 0);
Vector3	CollisionNormal(0, 0, 0);
Vector3	IntersectPoint(0, 0, 0);
Vector3	EndIntersectPoint(0, 0, 0);
int newCol=0;
// -------------------------------------------------------------------------
// updateTimer
//
// Update the timer.
// -------------------------------------------------------------------------

static void
updateTimer (struct glut_timer_t *t)
{
  t->last_time = t->current_time;
  t->current_time = glutGet (GLUT_ELAPSED_TIME) * 0.001f;
}
static void
idleVisible ()
{
  // Update the timer
  updateTimer (&timer);

  // Handle keyboard input
  //handleKeyboard (&keyboard);

  if (bAnimated)
    glutPostRedisplay ();
}
void InitializeGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				
	glClearDepth(1.0f);									
							
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
glActiveTextureARB	= (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");

	PROC procSwapInterval = wglGetProcAddress("wglSwapIntervalEXT");
	if(!glMultiTexCoord2fARB || !glActiveTextureARB) 
	{
		MessageBox(hWnd, "COULD NOT LOAD MULTI TEXTURNG", "error", MB_OK);
		return;
	}
	
	if (procSwapInterval)
	{
		SwapProc pS = (SwapProc)procSwapInterval;
		pS(0);
	}
	string dirname ("../data");
	string dirname1 ("/tris.md2");
	string dirname2 ("/weapon.md2");
    player = new Md2Player (dirname,dirname1,dirname2);

	player->setScale (1.0f);

	Sp3d.frameRate=7;

	player->setAnim ("stand");
	string dirname3 ("../data2");
	emty=new Md2Player (dirname3,dirname1,dirname2);
	const Md2Model *ref = player->playerMesh () ?
    player->playerMesh () : player->weaponMesh ();
	g_Model=new CModelMD3();
	g_Model->LoadModel(MODEL_PATH, MODEL_NAME);

	// Load the gun and attach it to our character
	g_Model->LoadWeapon(MODEL_PATH, GUN_NAME);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// When we get here, the character should have everything loaded.  Before going on,
	// we want to set the current animation for the torso and the legs.

	// Set the standing animation for the torso
	g_Model->SetTorsoAnimation("TORSO_STAND");

	// Set the walking animation for the legs
	g_Model->SetLegsAnimation("LEGS_IDLE");
	glEnable(GL_DEPTH_TEST);							
	glEnable(GL_TEXTURE_2D);
	
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glDepthFunc(GL_LEQUAL);	
	glShadeModel (GL_SMOOTH);
	//glutIdleFunc (idleVisible);
	
	glEnable (GL_CULL_FACE);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);

	glCullFace (GL_BACK);
}
void tPOINT()
{
	for(int i=0;i<Total;i++){
		float Fraction = bsp.Trace(&CollisionVertex[i], &Vector3(CollisionVertex[i]+IncidenceVertex[i]), 1);
		Vector3 theta=(IncidenceVertex[i]-CollisionVertex[i]);
		//float count =(((CollisionVertex.x-theta.x)+(CollisionVertex.z-theta.z))/2)/(((CollisionVertex.x+CollisionVertex.z))/2);
		float xyz = 0;//sin(count*360.0f*PI/180.0f)*64;
		GLUquadric* pquad =  gluNewQuadric();
		glPushMatrix();
		glTranslatef(CollisionVertex[i].x, CollisionVertex[i].y, CollisionVertex[i].z);
		gluSphere( pquad, 5,10,10);
		glPopMatrix();
		if(Fraction == 1.0f){
			CollisionVertex[i] =CollisionVertex[i]+IncidenceVertex[i];
			IncidenceVertex[i]=IncidenceVertex[i]-IncidenceVertex[i]/10.0f;
		}else
		{
			//IncidenceVertex[i]=IncidenceVertex[i]*-0.8f;
		}
	}
}
void redPoint(){
	Vector3	ReticlePos = cam.mPosition + ((cam.mView-cam.mPosition) * 20);
	glPushMatrix();
	glTranslatef(ReticlePos.x, ReticlePos.y, ReticlePos.z);
	glColor3f(1, 0,0);
	auxSolidSphere(.25/2);
	glPopMatrix();

}
void RenderScene(void)								
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	updateTimer (&timer);
if(cam.CheckKeyboard())
{
	cam.HandleCollisions();
}
	cam.HandleHeight();
	cam.CheckMouse(); //must be called after checkkeyboard and bsptrace
	cam.CheckShoot();
	gluLookAt(cam.mPosition.x, cam.mPosition.y, cam.mPosition.z, cam.mView.x, cam.mView.y, cam.mView.z, 0, .5, 0);
	

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	glEnable (GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glColor3f(1, 1, 1);

	frus.GetClipPlanes(); //Must get this before drawing the bsp 
	bsp.Render(bsp.FindLeaf(&cam.mPosition));
	FPS.UpdateFPS();
	
	glCullFace(GL_BACK);
	glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);	
				
	//tPOINT();
	display();	

	/*IncidenceVertex	=	CollisionVertex + ((IncidenceVertex - CollisionVertex) * bsp.Trace(&CollisionVertex, &IncidenceVertex, 1));
	glPushMatrix();
	glTranslatef(IncidenceVertex.x, IncidenceVertex.y, IncidenceVertex.z);
	auxSolidSphere(1);
	glPopMatrix();
	ReflectionVertex=CollisionVertex+((ReflectionVertex - CollisionVertex) * bsp.Trace(&CollisionVertex, &ReflectionVertex, 1));
	glPushMatrix();
	glTranslatef(ReflectionVertex.x, ReflectionVertex.y, ReflectionVertex.z);
	auxSolidSphere(1);
	glPopMatrix();
	CollisionNormal=CollisionVertex+((CollisionNormal - CollisionVertex) * bsp.Trace(&CollisionVertex, &CollisionNormal, 1));
	glPushMatrix();
	glTranslatef(CollisionNormal.x, CollisionNormal.y, CollisionNormal.z);
	auxSolidSphere(1);
	glPopMatrix();*/
		
//	glPushMatrix();
//	glTranslatef(EndIntersectPoint.x, EndIntersectPoint.y, EndIntersectPoint.z);
//	auxSolidSphere(20);
//	glPopMatrix();


	//glPushMatrix();
	//glTranslatef(IntersectPoint.x, IntersectPoint.y, IntersectPoint.z);
	//gluSphere( pquad, 1, 10, 10);
	//glPopMatrix();
	//glCullFace(GL_FRONT);	
	
	//DrawGLVector(&CollisionVertex, &IncidenceVertex, 1, 0, 0, 3);
	//DrawGLVector(&CollisionVertex, &ReflectionVertex, 0, 1, 0, 3);
	//DrawGLVector(&CollisionVertex, &CollisionNormal, 0, 0, 1, 3);
	glDisable (GL_DEPTH_TEST);
			
	glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
	glCullFace(GL_FRONT);	
	cam.mKeyboardInput = Vector3(0, 0, 0);
	//glutSwapBuffers ();
}





// -------------------------------------------------------------------------
// begin2D
//
// Enter into 2D mode.
// -------------------------------------------------------------------------

static void
begin2D ()
{
  //int width = glutGet (GLUT_WINDOW_WIDTH);
  //int height = glutGet (GLUT_WINDOW_HEIGHT);

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();

  glLoadIdentity ();
  glOrtho (0, 1.0f, 0,1.0f, -1.0f, 1.0f);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
}


// -------------------------------------------------------------------------
// end2D
//
// Return from 2D mode.
// -------------------------------------------------------------------------

static void
end2D ()
{
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode (GL_MODELVIEW);
}

// -------------------------------------------------------------------------
// gameLogic
//
// Perform application logic.
// -------------------------------------------------------------------------

static void
gameLogic ()
{
  // Calculate frame per seconds
  static double current_time = 0;
  static double last_time = 0;
  static int n = 0;

  n++;
  current_time = timer.current_time;

  if( (current_time - last_time) >= 1.0 )
    {
      fps = n;
      n = 0;
      last_time = current_time;
    }

  // Animate player
  if (bAnimated)
    {
      double dt = timer.current_time - timer.last_time;
      player->animate (Sp3d.frameRate * dt);
	  emty->animate (6 * dt);
    }
}


// -------------------------------------------------------------------------
// draw3D
//
// Render the 3D part of the scene.
// -------------------------------------------------------------------------

static void
draw3D ()
{
  // Clear window
  
  //glLoadIdentity ();
	glPushMatrix();
	
	//POINT MousePos;
	//GetCursorPos(&MousePos);
	//if(MousePos.x!=x){
	//	
	//	rot.y=-(MIDDLEX-MousePos.x);//?rot.y+1.0f:rot.y-1.0f;
	//	x=MousePos.x;
	//}
  bool i;
  static int curr=0;
  Vector3	ReticlePos = cam.mPosition + ((cam.mView-cam.mPosition) * 20);
  //Vector3 a=ReticlePos;//cam.mPosition-(cam.mPosition/20);
  // Perform camera transformations
  glTranslated (ReticlePos.x,ReticlePos.y-player->get_offset_st()/2,ReticlePos.z);
  
  glRotated (rot.x, 1.0f, 0.0f, 0.0f);
  glRotated (rot.y, 0.0f, 1.0f, 0.0f);
  glRotated (rot.z, 0.0f, 0.0f, 1.0f);

 

  if (bLightGL)
    glEnable (GL_LIGHTING);

  if (bTextured)
    glEnable (GL_TEXTURE_2D);

  if(Sp3d.run)
  {
	  player->setAnim ("run"); 
  }
  else if(Sp3d.jump)
  {
	  player->setAnim ("jump"); 
  }
  else if(Sp3d.atk)
  {
	  player->setAnim ("attack"); 
  }
  else
  {
	player->setAnim ("stand");
  }
  // Draw objects
  player->drawPlayerItp (bAnimated,
      static_cast<Md2Object::Md2RenderMode>(renderMode));

  i=player->getFrame();
  if(i&&!Sp3d.run){
	  /*if(++curr>10)
	  {*/
		  Sp3d.jump=0;
		  Sp3d.atk=0;
		  curr=0;
		  Sp3d.frameRate=7;
	  //}
  }
  Sp3d.run=0;
  //player->drawPlayerFrame (10,
  //    static_cast<Md2Object::Md2RenderMode>(renderMode));



  glDisable (GL_LIGHTING);
  glDisable (GL_TEXTURE_2D);
  //glDisable (GL_DEPTH_TEST);
  glPopMatrix ();
}

static void
draw3D2 ()
{
glPushMatrix();
	bool i;
	static int curr=0;

	Vector3 a(265.35272 ,165.03125 ,-1313.1431);
	
	glTranslated (a.x,a.y-emty->get_offset_st()/2,a.z);	//setting object position

	glEnable (GL_LIGHTING);			//open lighting

	glEnable (GL_TEXTURE_2D);		//graphic 2d textrue

	emty->setAnim ("stand");		//setting animated

	// Draw objects
	emty->drawPlayerItp (bAnimated,static_cast<Md2Object::Md2RenderMode>(renderMode));

	glDisable (GL_LIGHTING);		//cloese light

	glDisable (GL_TEXTURE_2D);		//disable texture

glPopMatrix ();
}
// -------------------------------------------------------------------------
// draw2D
//
// Render the 2D part of the scene.
// -------------------------------------------------------------------------

static void
draw3D3 ()
{
glPushMatrix();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Vector3 a(-184.03329 ,190.42139 ,-851.15192);
	//glRotated (0, 0.0f, 1.0f, 0.0f);
	glTranslated (a.x,a.y-34,a.z);	//setting object position
	g_Model->DrawModel();		
  //draw2D ();
glPopMatrix ();
}
// -------------------------------------------------------------------------
// display
//
// Render the main scene to the screen.
// -------------------------------------------------------------------------

static void
display ()
{
  
  gameLogic ();

  draw3D ();

  draw3D2 ();

  draw3D3 ();
}
