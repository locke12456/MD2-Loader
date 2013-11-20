#include "FPS.h"
#include "Defines.h"
extern	std::ofstream	trace;
void FPS_t::Start()
{
	LastTime = GetTickCount();
	frames = 0;
	TimeFrac = 0;
	LastFramesTime = GetTickCount();
}

void	FPS_t::UpdateFPS()
{
		++frames;
		CurrentTime = GetTickCount();
		TimePassed = CurrentTime - LastFramesTime;

		if( TimePassed > 0)
			TimeFrac = TimePassed * .001f;

		LastFramesTime	=	CurrentTime;
		if(((CurrentTime - LastTime) >= 1000)) 
		{
			LastTime = CurrentTime;
			sprintf(display, "FPS: %d", (int)frames);
			SetWindowText(hWnd, display);
			frames = 0;
		}
}
