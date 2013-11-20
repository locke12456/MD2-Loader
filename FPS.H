class FPS_t
{
public:
	FPS_t(){}
	void Start();

	void UpdateFPS();

	char display[50];
	//time passed / 1000

	float	ThisFramesTime;
	float	LastFramesTime;
	float	TimeFrac; //ThisFramesTime-LastFramesTime / 1000
	float	TimePassed;
	
	int LastTime;
	int CurrentTime;
	int frames;
};
