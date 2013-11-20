//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glu32.lib")
//#pragma comment(lib, "glaux.lib")
#include "Objects.h"
#include "TextureManager.h"
#define	WINDOWS_BUILD	1
#define	INFINITY 1000


HDC			DEVICE_CONTEXT=0;
HGLRC		RENDERING_CONTEXT = 0;
HWND		hWnd=NULL;
HINSTANCE	hInstance;	

int	mousedown;
extern	BSP	bsp;
extern	FPS_t	FPS;
extern	PFNGLMULTITEXCOORD2FARBPROC	glMultiTexCoord2fARB;
extern	PFNGLACTIVETEXTUREARBPROC	glActiveTextureARB;

void SizeWindow(float width, float height)	
{
	if (!height)									
		height=1;									
	
	glViewport(0,0,width,height);				
	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();
	//FOVY necessary to get a FOVX of 90 degrees...approximately what the Human eye has, I think 
	gluPerspective(67.5,(float)width/(float)height,CLOSE_CLIP_PLANE,FAR_CLIP_PLANE);
	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();									
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT	uMsg,WPARAM	wParam,LPARAM lParam)			
{
	switch (uMsg)									
	{	
	case	WM_LBUTTONDOWN:
		{
			mousedown = 1;
			break;
		}
	case	WM_LBUTTONUP:
		{
			mousedown = 0;
			break;
		}
		//Only need these in case esc is pressed, otherwise don't put keyboard stuff inwinmain
		case WM_CLOSE:								
		{
			PostQuitMessage(0);						
			return 0;								
		}
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,	HINSTANCE	hPrevInstance,LPSTR lpCmdLine,int nCmdShow)			
{
	if(hPrevInstance)
		return 0;	//only want one instance
	
	srand(GetTickCount());

	MSG	MESSAGE;							
	
	BOOL	WINDOWSHASNTCRASHED=1;								
	
			
	WNDCLASS	windowclass;						
	DWORD		MoreWindowsStyle;				
	DWORD		dwStyle;				
	RECT		GL_WINDOW;				
	GL_WINDOW.left=(int)0;			
	GL_WINDOW.right=(int)SCREEN_WIDTH;	
	GL_WINDOW.top=(int)0;				
	GL_WINDOW.bottom=(int)SCREEN_HEIGHT;
windowclass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
windowclass.lpfnWndProc		= (WNDPROC) WndProc;				
windowclass.cbClsExtra		= 0;								
windowclass.cbWndExtra		= 0;								
windowclass.hInstance		= hInstance;						
windowclass.hIcon			= LoadIcon(NULL, IDI_ERROR);			
windowclass.hCursor			= LoadCursor(NULL, IDC_CROSS);			
windowclass.hbrBackground	= NULL;									
windowclass.lpszMenuName		= NULL;									
windowclass.lpszClassName	= "BSP";								
RegisterClass(&windowclass);
DEVMODE fullscreensettings;				
fullscreensettings.dmSize=sizeof(fullscreensettings);		
fullscreensettings.dmPelsWidth	= SCREEN_WIDTH;			
fullscreensettings.dmPelsHeight	= SCREEN_HEIGHT;		
fullscreensettings.dmBitsPerPel	= 32;					
fullscreensettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
ChangeDisplaySettings(&fullscreensettings,CDS_FULLSCREEN);	
MoreWindowsStyle=WS_EX_APPWINDOW;								
dwStyle=WS_POPUP;										
ShowCursor(FALSE);										
AdjustWindowRectEx(&GL_WINDOW, dwStyle, FALSE, MoreWindowsStyle);		
hWnd=CreateWindowEx(MoreWindowsStyle,"BSP","GL",dwStyle |WS_CLIPSIBLINGS |WS_CLIPCHILDREN,0, 0,	SCREEN_WIDTH, SCREEN_HEIGHT,NULL,NULL,hInstance,NULL);					

//this structure copied from NeHe's, I didn't want to set up anything incorrectly
	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),			
		1,										
		PFD_DRAW_TO_WINDOW |					
		PFD_SUPPORT_OPENGL |					
		PFD_DOUBLEBUFFER,						
		PFD_TYPE_RGBA,							
		32,									
		0, 0, 0, 0, 0, 0,					
		0,									
		0,									
		0,									
		0, 0, 0, 0,						
		32,				
		0,								
		0,								
		PFD_MAIN_PLANE,					
		0,								
		0, 0, 0							
	};
	
DEVICE_CONTEXT=GetDC(hWnd);
GLuint		PixelFormat;
PixelFormat=ChoosePixelFormat(DEVICE_CONTEXT,&pfd);	
SetPixelFormat(DEVICE_CONTEXT,PixelFormat,&pfd);
RENDERING_CONTEXT=wglCreateContext(DEVICE_CONTEXT);
wglMakeCurrent(DEVICE_CONTEXT,RENDERING_CONTEXT);
ShowWindow(hWnd,SW_SHOW);						
SetForegroundWindow(hWnd);						
SetFocus(hWnd);									
SizeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);	
InitializeGL();
if(!bsp.LoadBSP("GameTutorialsBSP.bsp")) 
		return 0;
SetCursorPos(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
FPS.Start();

	while(WINDOWSHASNTCRASHED)								
	{
		if (PeekMessage(&MESSAGE,NULL,0,0,PM_REMOVE))
		{
			if (MESSAGE.message==WM_QUIT)				
			{
				break;						
			}
			else									
			{ 
				TranslateMessage(&MESSAGE);				
				DispatchMessage(&MESSAGE);				
			}
		}
		else										
		{
				if(GetKeyState(VK_ESCAPE) & 0x80)
				{
					Texture2DManager::kill ();
					break;
				}
				else							
				{
					RenderScene();

					SwapBuffers(DEVICE_CONTEXT);			
				}
			}
		}

	return	0;					
}

