#include "LoadTexture.h"
#define TGA_RLE		10		// This tells us that the targa is Run-Length Encoded (RLE)
std::ofstream	textureerror("textureerror.txt");
eTexTypes	GetTexType(char * FileName)
{
	FILE	*pFile = NULL;
	char	*TGA = new char[(strlen(FileName) + 4)];
	strcpy(TGA, FileName);
	strcat(TGA, ".tga");
	pFile = fopen(TGA, "rb");
	if(pFile)
		return eTGA;

	char	*JPG = new char[(strlen(FileName)+4)];
	strcpy(JPG, FileName);
	strcat(JPG, ".jpg");
	pFile = fopen(JPG, "rb");
	if(pFile)
		return eJPG;

	char	*BMP = new char[(strlen(FileName) + 4)];
	strcpy(BMP, FileName);
	strcat(BMP, ".bmp");
	pFile = fopen(BMP, "rb");
	if(pFile)
		return eBMP;

	return eSymbolic; //if we get here this texture has no extension and must be symbolic
}

//Only works when targas are used, does not work when bitmaps are used
UINT	LoadBMP(char * FileName)
{
	UINT	texture = 0;

	if(!FileName) 
	{
		textureerror << "could not find: " << FileName << "\n";
		return NULL;
	}

	AUX_RGBImageRec	*pImage;
	
	FILE	*pFile = fopen(FileName, "rb");
	
	if(!pFile)
	{
		textureerror << "could not find: " << FileName << "\n";
		return NULL; 
	}

	pImage = auxDIBImageLoad(FileName);

	if(!pImage)
	{		
		return NULL;
	}

	texture = GenerateInternalTexture(pImage->data, 3, pImage->sizeX, pImage->sizeY, GL_RGB);

	if (pImage)										
	{
		if (pImage->data)						
		{
			free(pImage->data);					
		}

		free(pImage);								
	}
	fclose(pFile);
	return texture;
}

//FIXME: emptyl
UINT	LoadJPG(char * FileName)
{
	return 5;
}

UINT	LoadTGA(char	*FileName)
{
	int	i; //counter
	WORD	width, height;
	byte	*data	=	NULL;
	byte	type, bits, length;

	int		rowlength, channels, filesize;
	byte	*pBuffer = NULL;
	byte	*ptr = NULL;
	FILE	*pFile = fopen(FileName, "rb");

	if(!pFile)
	{
		textureerror << "could not find: " << FileName << "\n";
		return	NULL;	//FIXME: some sort of error log or notification needed	
	}

	fseek(pFile, 0, SEEK_END);
	filesize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	pBuffer = new byte[filesize];
	fread(pBuffer, 1, filesize, pFile);
	fclose(pFile);
	
	ptr = pBuffer;
	length	=	*(byte*)ptr;
	ptr+=2;

	type	=	*(byte*)ptr;
	ptr += 10;

	width	=	*(WORD*)ptr;
	ptr += sizeof(WORD);

	height	=	*(WORD*)ptr;
	ptr	+= sizeof(WORD);

	bits	=	*(byte*)ptr;
	ptr++;
	ptr += length+1;

	if(bits == 16)
	{
		MessageBox(NULL, "LoadTGA: Doesn't support 16bit","ERROR", MB_OK);
		return NULL;
	}
		//Possible engine design idea: have FILE_MANAGER structure to take care of these things
	channels = bits / 8;
	rowlength = channels * width;//length of a row in bytes
	data = new byte[rowlength * height];
	
unsigned	int		x, y;
if(type != TGA_RLE)
{
	if(channels == 3)
	{
		for(i = 0; i < height; i++)
		{
			for(y = 0; y < rowlength; y++)
			{
				data[(i*rowlength)+y] = *(byte*)ptr;
				ptr++;
			}	
		}
		GLenum	GLtype = GL_BGR_EXT;
		return	GenerateInternalTexture(data, channels, width, height, GLtype);
	}
	else
	{
		if(channels == 4)
		{
			for(i = 0; i < height; i++)
			{
				for(y = 0; y < rowlength; y++)
				{
					data[(i*rowlength)+y] = *(byte*)ptr;
					ptr++;
				}
			}	
		}
		GLenum	GLtype	=	GL_BGRA_EXT;
		
		ptr = NULL;
		delete	[]	pBuffer;
		delete	ptr;

		return	GenerateInternalTexture(data, channels, width, height, GLtype);
	}
	//FIXME: make sure all of the data is cleaned up! 
}
else	if(type == TGA_RLE) //NOTE: LoadTGA_RLE must delete data when done!
	return	LoadTGA_RLE(pBuffer, ptr, bits, channels, width, height, data);
}

UINT	LoadTGA_RLE(byte*pBuffer, byte*ptr, byte	bits, byte	channels, WORD width, WORD height, byte	*data)
{
		byte *pColor = new byte[channels];
		int	pixelsleft = 0;
		int	colorsread = 0;
		int	pixelsread = 0;
		int	totalpixels = width * height;
		unsigned	int	y; //counter 
		
		//FIXME: must have an error logging system! 
		if(!ptr)
			return	NULL;
		if(!pBuffer)
			return	NULL;
		while(pixelsread < totalpixels)
		{
			pixelsleft = *(byte*)ptr;
			ptr++;

			if(pixelsleft < 128)
			{
				pixelsleft++;

			if(channels == 4)
			{
					while(pixelsleft)
					{
						for(y = 0; y < channels; y++)
						{
							pColor[y] = *(byte*)ptr;
							ptr++;
						}
						data[colorsread + 0] = pColor[0];	
						data[colorsread + 1] = pColor[1];
						data[colorsread + 2] = pColor[2];	
						data[colorsread + 3] = pColor[3];

						pixelsread++;
						pixelsleft--;
						colorsread += channels;
					}
				}
				else	if(channels == 3)
				{
						while(pixelsleft)
						{
							for(y = 0; y < channels; y++)
							{
								pColor[y] = *(byte*)ptr;
								ptr++;
							}
							data[colorsread + 0] = pColor[0];	
							data[colorsread + 1] = pColor[1];
							data[colorsread + 2] = pColor[2];	

							pixelsread++;
							pixelsleft--;
							colorsread += channels;
						}
				}
			}
			else
			{
				pixelsleft -= 127;
				for(y = 0; y < channels; y++)
				{
					pColor[y] = *(byte*)ptr;
					ptr++;
				}

				if(channels == 4)
				{
					while(pixelsleft)
					{
						data[colorsread + 0] = pColor[0];
						data[colorsread + 1] = pColor[1];
						data[colorsread + 2] = pColor[2];
						data[colorsread + 3] = pColor[3];
						pixelsread++;
						pixelsleft--;
						colorsread += channels;
					}
				}
				else	if(channels == 3)
				{
					while(pixelsleft)
					{
						data[colorsread + 0] = pColor[0];
						data[colorsread + 1] = pColor[1];
						data[colorsread + 2] = pColor[2];
						pixelsread++;
						pixelsleft--;
						colorsread += channels;
					}
				}
				
			}
		}
		
		ptr=NULL;
		delete	[]	pBuffer;
		delete	ptr;
		
		GLenum	GLtype;
		if(channels == 4)
			GLtype = GL_BGRA_EXT;
		if(channels == 3)
			GLtype = GL_BGR_EXT;
		return	GenerateInternalTexture(data, channels, width, height, GLtype);
	}

UINT	GenerateInternalTexture(byte * data, int channels, int width, int height, GLenum type)
{
	UINT	texture;
	glGenTextures(1, &texture);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texture);

	gluBuild2DMipmaps(GL_TEXTURE_2D, channels,width,height, type, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	return texture;
}
