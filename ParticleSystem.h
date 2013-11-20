#define NUM_PARTICLES	1000
#define PARTICLE_SIZE   7

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Attribute locations
   GLint  lifetimeLoc;
   GLint  startPositionLoc;
   GLint  endPositionLoc;
   
   // Uniform location
   GLint timeLoc;
   GLint colorLoc;
   GLint centerPositionLoc;
   GLint samplerLoc;

   // Texture handle
   GLuint textureId;

   // Particle vertex data
   float particleData[ NUM_PARTICLES * PARTICLE_SIZE ];

   // Current time
   float time;

} UserData;

class Particle
{
public:
	Particle();
private:

	ESContext esContext;
    UserData  Userdata;
};