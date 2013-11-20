#ifndef LOADTEX_H
#define LOADTEX_H

#include "Defines.h"

enum eTexTypes
{
	eTGA = 0, 
	eJPG,
	eBMP,
	ePCX, //quake2 format for textures
	eSymbolic //symbolic texture name for defining entities and shit
};

eTexTypes	GetTexType(char	*FileName);

GLuint	LoadGLTextures(char *FileName);

UINT	LoadBMP(char * FileName);
UINT	LoadTGA(char * FileName);
UINT	LoadTGA_RLE(byte*pbuffer, byte *ptr, byte	bits, byte	channels, WORD width, WORD height, byte *data);

UINT	LoadJPG(char * FileName);

//type = GL_RGB or GL_RGB_EXT 
UINT	GenerateInternalTexture(byte * data, int channels, int width, int height, GLenum type);

#endif

