#include "Defines.h"
#include "Quake3BSP.h"								
#include "LoadTexture.h"
#include "Glext.h"
#include "Camera.h"
#include "Frustum.h"
#include "FPS.h"
//FIXME: ugly externs and globals and defines, again :(
#define	ENDL "\n"
#define	LMAPSIZE	(128*128*3)
int		closestplane; 
float	RADIUS = 10.0f; //not sure what this should be defined as
#define EPSILON .03125

int		LastPlaneCrossed;
extern	Vector3	BoxMaxs;

extern PFNGLMULTITEXCOORD2FARBPROC	glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC	glActiveTextureARB;
extern Camera cam;
extern Frustum frus;
extern FPS_t FPS;
std::ofstream trace("Trace.txt");
std::ofstream	textures("textures.txt");

struct	pixelf
{
	pixelf(){}
	pixelf(float rt,float gt,float bt):r(rt),g(gt),b(bt){}
	float	r,g,b;
};

struct	pixelfa
{
	pixelfa(){}
	pixelfa(float rt,float gt,float bt,float at):r(rt),g(gt), b(bt), a(at){}
	float	r,g,b,a;
};

//To make Y up swap the Y and Z values and negate new Z
void	BSP::MakeYUp(Vector3	* a)
{
	float	temp = a->z;
	a->z = -a->y;
	a->y = temp;
}

//for leaf bounding boxes
void	BSP::MakeYUp(tVector3i	*a)
{
	float	temp = a->z;
	a->z = -a->y;
	a->y = temp;
}

//byte [128][128][3]
//equals to rgb for 128 x 128 map
UINT	BSP::GenLightmap(tBSPLightmap * lightmap)
{
	UINT	texture = 0;
	glGenTextures(1, &texture);

	//FIXME: look more into how these texture functions work, possible optimizations anywhere?

	//this function determines how the pixel data is stored in memory, not significant unless you are
	//reading the data from screen for a screenshot or something
	// the second parameter is how many bytes between rows of pixel memory
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//creates/selects the texture
	glBindTexture(GL_TEXTURE_2D, texture);

	pixelf pixel;
	float	max,	temp;

	//touch every single pixel and manipulate its color while keeping things in range
	byte	*ImageData	=	&lightmap->imageBits[0][0][0];
	for(int i = 0; i < LMAPSIZE; i+=3)
	{
		max = 1.0f;
		temp	=	0.0f;
pixel=pixelf(((float)ImageData[i]*3.1)/255.0f,((float)ImageData[i+1]*3.1)/255.0f,((float)ImageData[i+2]*3.1)/255.0f);
	
	if(pixel.r > 1.0f)
		if((1.0f/pixel.r) < max)
			max = 1.0f/pixel.r;

	if(pixel.g > 1.0f)
		if((1.0f/pixel.g) < max)
			max = 1.0f/pixel.g;

	if(pixel.b > 1.0f)
		if((1.0f/pixel.b) < max)
			max = 1.0f/pixel.b;
	
		max*=255;
pixel=pixelf(pixel.r*max, pixel.g*max, pixel.b*max);

		ImageData[i] = (byte)pixel.r;
		ImageData[i+1] = (byte)pixel.g;
		ImageData[i+2] = (byte)pixel.b;
	}
	//mip maps generate arrays of the same texture information so that it uses lower resolution textures
	//when you are far away and higher resolution textures when you are up close
	//because our lightmaps are 128x128 that is static to this function
	//second parameter is number of channels, rgb (+a if 4 channels)
gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 128, 128, GL_RGB, GL_UNSIGNED_BYTE, lightmap->imageBits);

	//sets up various texturing attributes, in this case how to magnify the textures (linear magnification)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	return texture;
}

bool	BSP::LoadBSP(char	*filename) 
{
	
	char	*FileError = "LoadBSP: Could Not Load File: ";
	int i;

	FILE	*pFile = fopen(filename, "rb");
	if(!pFile)
	{
		char	*message = new char[strlen(FileError) + strlen(filename)];
		strcpy(message, FileError);
		strcat(message, filename);
		MessageBox(NULL, message, "ERROR", MB_OK);
		return false;
	}

	//FIXME: filesize function
	fseek(pFile, 0, SEEK_END);
	filesize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	mpBuffer = new byte[filesize];
	fread(mpBuffer, 1, filesize, pFile);
	fclose(pFile);
	ptr = mpBuffer;
	tBSPHeader	*Header = (tBSPHeader*)ptr;

	ptr += sizeof(tBSPHeader);
	
	for(i = 0; i < kMaxLumps; i++)
	{
		tBSPLump	*tempLump = (tBSPLump*)ptr;
		Lumps[i].length = tempLump->length;
		Lumps[i].offset = tempLump->offset;
		ptr += sizeof(tBSPLump);
	}

	LoadGLTextures(filename); //loads textures as well as lightmaps
	
	mNumVerts = Lumps[kVertices].length / VERTEX_SIZE;
	mpVertices = new tBSPVertex[mNumVerts];

	ptr = mpBuffer;
	ptr += Lumps[kVertices].offset;

	for(i = 0; i < mNumVerts; i++)
	{
		tBSPVertex	*tempVert = (tBSPVertex*)ptr;
		memcpy(&mpVertices[i], tempVert, VERTEX_SIZE);
		MakeYUp(&mpVertices[i].vPosition);
		mpVertices[i].vTextureCoord.v *= -1;
		ptr += VERTEX_SIZE;
	}
	
	mNumFaces = Lumps[kFaces].length / FACE_SIZE;

	mpFaces = new tBSPFace[mNumFaces];
	ptr = mpBuffer;
	ptr += Lumps[kFaces].offset;

	for(i = 0; i < mNumFaces; i++)
	{
		tBSPFace	*tempFace = (tBSPFace*)ptr;
		memcpy(&mpFaces[i], tempFace, FACE_SIZE);
		ptr += FACE_SIZE;
	}

	mNumNodes	=	Lumps[kNodes].length / NODE_SIZE;
	mpNodes	=	new	tBSPNode[mNumNodes];

	ptr = mpBuffer;
	ptr += Lumps[kNodes].offset;
	
	for(i = 0; i < mNumNodes; i++)
	{
		tBSPNode	*tempNode	= (tBSPNode*)ptr;
		memcpy(&mpNodes[i], tempNode, NODE_SIZE);
		ptr += NODE_SIZE;
	}

	mNumLeafs = Lumps[kLeafs].length / LEAF_SIZE;
	mpLeafs = new tBSPLeaf[mNumLeafs];

	ptr = mpBuffer;
	ptr += Lumps[kLeafs].offset;

	for(i = 0; i < mNumLeafs; i++)
	{
		tBSPLeaf	*tempLeaf	= (tBSPLeaf*)ptr;
		memcpy(&mpLeafs[i], tempLeaf, LEAF_SIZE);
		MakeYUp(&mpLeafs[i].min);
		MakeYUp(&mpLeafs[i].max);
		ptr += LEAF_SIZE;
	}

	mNumLeafFaces	= Lumps[kLeafFaces].length / sizeof(int);
	mpLeafFaces	= new int[mNumLeafFaces];
	
	ptr = mpBuffer;
	ptr += Lumps[kLeafFaces].offset;

	for(i = 0; i < mNumLeafFaces; i++)
	{
		int	*tempLeafFace = (int*)ptr;
		mpLeafFaces[i] = *tempLeafFace;
		ptr += sizeof(int);
	}

	mNumPlanes = Lumps[kPlanes].length / PLANE_SIZE;
	mpPlanes = new tBSPPlane[mNumPlanes];

	ptr = mpBuffer;
	ptr += Lumps[kPlanes].offset;

	for(i = 0; i < mNumPlanes; i++)
	{
		tBSPPlane	*tempPlane = (tBSPPlane*)ptr;
		memcpy(&mpPlanes[i], tempPlane, PLANE_SIZE);
		MakeYUp(&mpPlanes[i].vNormal);
		ptr += PLANE_SIZE;
	}

//FIXME: wanted to not have to use a FILE in order to extract this part of the file, but using the buffer doesn't work :(

	if(Lumps[kVisData].length) 
	{
		FILE	*fp = fopen(filename, "rb");
		fseek(fp, Lumps[kVisData].offset,SEEK_SET);
		fread(&mVisData.numOfClusters, sizeof(int), 1, fp);
		fread(&mVisData.bytesPerCluster, sizeof(int), 1, fp);
		mVisData.pBitsets = new byte[mVisData.numOfClusters * mVisData.bytesPerCluster];
		fread(mVisData.pBitsets, 1, mVisData.numOfClusters * mVisData.bytesPerCluster, fp);
		fclose(fp);
	}
		else
			mVisData.pBitsets = NULL; //if there is no bitset stuff

	//Extremely fast
	LoadBrushes(); 
	CalculateParentNodes(); 
	//FIXME: make sure to delete all allocated memory and close all files!
	delete[]	mpBuffer;
	ptr = NULL;
	return true;
}


bool	BSP::IsLeafPV(int test, int current) //(Leaf being tested , Leaf camera is in)
{
	if(!mVisData.pBitsets || current < 0) return 1; //done in GT so I do it too, just to be safe

	//This just accessing the current byte that stores the bit for the test cluster, but this part doesn't actually
	//find that bit (just the byte it resides in)
	int location = (current * mVisData.bytesPerCluster) + (test / 8); //if there is a decimal in this part it gets rounded off 
	byte visSet = mVisData.pBitsets[location];
	//this actually determins if the bit that stores the test cluster is 1
	//it does this by taking the byte you have found above and modding (%) it with 7 (the highest power in 8 bits)
	//to determine if that bit that stores the test cluster is a 1 or a 0
	bool result = visSet & (1 << ((test) & 7));

	return result;
}

/*
 Ax + By + Cz - D = 0;
 Positive if in front of plane
 Negative if behind plane
*/
bool	BSP::InFront(Vector3 * a, tBSPPlane	*p)
{
	if(DotProduct(a, &p->vNormal) - p->d >= 0) //must subtract, because dist calculated by DP
											   //Normal, Point instead of negative normal 
		return true;
	else
		return false;
}

/*
 to find leaf camera is in, start at root node and determine if in front of back of node
*/
//This seems to be working, doesn't seem to be any problems with it


int	BSP::FindLeaf(Vector3 * a)
{
int i = 0;	//leaf index, starts at root node, steps through until leaf index is negative, in which case
				//the real index is -(i + 1)
	while(i >= 0)
	{
		if(DotProduct(a, &mpPlanes[mpNodes[i].plane].vNormal)>=mpPlanes[mpNodes[i].plane].d)
		
			{
				i = mpNodes[i].front;
			}
		else
			{
				i = mpNodes[i].back;
			}
	}
		return -(i+1);
}
int	BSP::FindLeaf(Vector3 * a, std::vector<int>	*b)
{
int i = 0;	//leaf index, starts at root node, steps through until leaf index is negative, in which case
				//the real index is -(i + 1)
	while(i >= 0)
	{
		if(InFront(a, &mpPlanes[mpNodes[i].plane]))
			{
				i = mpNodes[i].front;
				b->push_back(i);
			}
		else
			{
				i = mpNodes[i].back;
				b->push_back(i);
			}
	}
		return -(i+1);
}

int	BSP::FindLeafFromCurrent(Vector3	*a, int	Current)
{
	int	i = Current; //starting node
	if(i < 0)
		return Current; //already in a leaf, must be a node
	while(i >= 0)
	{
		if(InFront(a, &mpPlanes[mpNodes[i].plane]))
		{
			i = mpNodes[i].front;
		}
		else
		{
			i = mpNodes[i].back;
		}
	}
	return -(i+1);
}

//To render this leaf loop through all of the faces that it owns
//leaf holds indices into leafface array, which in turn holds indices into face array
void	BSP::RenderFace(int i)
{
		if(mpFaces[i].type != 1 || mpFaces[i].LastFrameDrawn == FPS.frames)
			return; //if not face polygon or already drawn
		mpFaces[i].LastFrameDrawn = FPS.frames;
		
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D); //for some reason when this is not included it does not work
		glBindTexture(GL_TEXTURE_2D, mpGLTextures[mpFaces[i].textureID]);
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mpLightMaps[mpFaces[i].lightmapID]);
		
		glBegin(GL_TRIANGLE_FAN);
			
		for(int j = 0; j < mpFaces[i].numOfVerts; j++)
		{
			Vector3	*tempVert1 = &mpVertices[mpFaces[i].startVertIndex+j].vPosition;
			Vector2 *tempTex1 = &mpVertices[mpFaces[i].startVertIndex+j].vTextureCoord;
			Vector2 *tempTex2 = &mpVertices[mpFaces[i].startVertIndex+j].vLightmapCoord;
			
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, tempTex1->u, tempTex1->v);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, tempTex2->u, tempTex2->v);
			glVertex3f(tempVert1->x, tempVert1->y, tempVert1->z);
		}
	glEnd();
}

void	BSP::RenderLeaf(tBSPLeaf*a) 
{
	for(int i = 0; i < a->numOfLeafFaces; i++)
	{
		RenderFace(mpLeafFaces[a->leafface+i]);
	}
}

void	BSP::Render(int	LeafIndex)
{
		int i = 0;
		int current = mpLeafs[LeafIndex].cluster;
for( i = 0; i < mNumLeafs; i++)
	{
	if(IsLeafPV(mpLeafs[i].cluster, current)) 	
		{
		if(frus.BBInFrustum(mpLeafs[i].min, mpLeafs[i].max))
			{
				RenderLeaf(&mpLeafs[i]);
			}
		}
	}	
}
void	BSP::DumpTree(int	NodeIndex)
{
	if(NodeIndex < 0)
		return;
	DumpTree(mpNodes[NodeIndex].front);
	DumpTree(mpNodes[NodeIndex].back);

}

void	BSP::CalculateParentNodes()
{
	if(mpNodes[0].front >= 0)
		SetMeAsParent(mpNodes[0].front, 0); 
	if(mpNodes[0].back >= 0)	
		SetMeAsParent(mpNodes[0].back, 0); 
}

void	BSP::SetMeAsParent(int Dest, int ParentIndex) 
{
	tBSPNode	*Current = &mpNodes[Dest];
	Current->parent = ParentIndex;
	
		if(Current->front >= 0) 
			SetMeAsParent(Current->front, Dest); 
		else 
		{
			int	i = -(Current->front + 1);
			mpLeafs[i].parent = Dest;
		}
	
		if(Current->back >= 0)
			SetMeAsParent(Current->back, Dest);
		else
		{
			int x = -(Current->back + 1);
			mpLeafs[x].parent = Dest;
		}
}

void	BSP::DrawBBox(tVector3i	vec1, tVector3i	vec2)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor4f(1, 0, 0, .5);
	glDepthFunc(GL_GEQUAL);
	glBegin(GL_QUADS);
		glVertex3f(vec1.x, vec1.y, vec1.z);
		glVertex3f(vec2.x, vec1.y, vec1.z);
		glVertex3f(vec2.x, vec2.y, vec1.z);
		glVertex3f(vec1.x, vec2.y, vec1.z);
		
		glVertex3f(vec1.x, vec1.y, vec2.z);
		glVertex3f(vec2.x, vec1.y, vec2.z);
		glVertex3f(vec2.x, vec2.y, vec2.z);
		glVertex3f(vec1.x, vec2.y, vec2.z);

		glVertex3f(vec2.x, vec1.y, vec1.z);
		glVertex3f(vec2.x, vec1.y, vec2.z);
		glVertex3f(vec2.x, vec2.z, vec2.z);
		glVertex3f(vec2.x, vec2.y, vec1.z);

		glVertex3f(vec1.x, vec1.y, vec1.z);
		glVertex3f(vec1.x, vec1.y, vec2.z);
		glVertex3f(vec1.x, vec2.z, vec2.z);
		glVertex3f(vec1.x, vec2.y, vec1.z);

		glVertex3f(vec1.x, vec2.y, vec1.z);
		glVertex3f(vec1.x, vec2.y, vec2.z);
		glVertex3f(vec2.x, vec2.y, vec2.z);
		glVertex3f(vec2.x, vec2.y, vec1.z);

		glVertex3f(vec1.x, vec1.y, vec1.z);
		glVertex3f(vec1.x, vec1.y, vec2.z);
		glVertex3f(vec2.x, vec1.y, vec2.z);
		glVertex3f(vec2.x, vec1.y, vec1.z);
	glEnd();	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LEQUAL);
	glColor3f(1, 1, 1);
}

void	BSP::DeleteTextures()
{
	if(mpGLTextures)
		delete[]	mpGLTextures;
	if(mpLightMaps)
		delete[]	mpLightMaps;
}

void	BSP::TouchTexture(char	*filename)
{
	byte	*tempbuffer = NULL;
	int		filesize;
	FILE	*pFile=NULL;
	FILE	*pOut=NULL;
	eTexTypes	temp = GetTexType(filename);
		switch(temp)
		{
		case eTGA:
			strcat(filename, ".tga");
			pFile	=	fopen(filename, "rb");
			fseek(pFile, 0, SEEK_END);
			filesize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);
			tempbuffer = new byte[filesize];
			fread(tempbuffer, 1, filesize, pFile);
			fclose(pFile);
			strcat(filename, "WWW");
			pOut = fopen(filename, "w");
			fwrite(tempbuffer, 1, filesize, pOut);
			fclose(pFile);
			fclose(pOut);
			tempbuffer	=	NULL;
			delete[]	tempbuffer;
			break;
		case eJPG:
			strcat(filename, ".jpg");
			pFile	=	fopen(filename, "rb");
			fseek(pFile, 0, SEEK_END);
			filesize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);
			tempbuffer = new byte[filesize];
			fread(tempbuffer, 1, filesize, pFile);
			fclose(pFile);
			strcat(filename, "WWW");
			pOut = fopen(filename, "w");
			fwrite(tempbuffer, 1, filesize, pOut);
			fclose(pFile);
			fclose(pOut);
			tempbuffer	=	NULL;
			delete[]	tempbuffer;
			break;
		case eBMP:
			{
				char	*bmptex = new char[strlen(filename + 4)];
				strcpy(bmptex, filename);
				strcat(bmptex, ".bmp");
			pFile	=	fopen(filename, "rb");
			fseek(pFile, 0, SEEK_END);
			filesize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);
			tempbuffer = new byte[filesize];
			fread(tempbuffer, 1, filesize, pFile);
			fclose(pFile);
			strcat(filename, "WWW");
			pOut = fopen(filename, "w");
			fwrite(tempbuffer, 1, filesize, pOut);
			fclose(pFile);
			fclose(pOut);
			tempbuffer = NULL;
			delete[]	tempbuffer;
			break;
			}

		default:
			break;	//FIXME: do anything special here?
		}
}
void	BSP::LoadGLTextures(char	*FileName)
{
	FILE	*pFile = fopen(FileName, "rb");
	
	if(!pFile)
	{
		MessageBox(NULL, "COULD NOT LOAD TEXTURES", "ERROR", MB_OK);
		return;
	}
	
	DeleteTextures();
	int	FileSize;
	byte	*Buffer = NULL;
	byte	*ptr = NULL;

	fseek(pFile, 0, SEEK_END);
	FileSize = ftell(pFile);
	Buffer = new byte[FileSize];
	fseek(pFile, 0, SEEK_SET);
	fread(Buffer, 1, FileSize, pFile);
	fclose(pFile);

	mNumTextures = Lumps[kTextures].length / TEXTURE_SIZE;
	mpGLTextures = new	UINT[mNumTextures];
	mpTextures	 = new	tBSPTexture[mNumTextures];

	ptr = Buffer;
	ptr += Lumps[kTextures].offset; 
	int i;
	for(i = 0; i < mNumTextures; i++)
	{
		tBSPTexture	*temptexture = (tBSPTexture*)ptr;
		mpTextures[i].contents	=	temptexture->contents;
		mpTextures[i].flags		=	temptexture->flags;
		eTexTypes	temp = GetTexType(temptexture->strName);
		switch(temp)
		{
		case eTGA:
			strcat(temptexture->strName, ".tga");
			mpGLTextures[i] = LoadTGA(temptexture->strName);
			break;
		case eJPG:
			strcat(temptexture->strName, ".jpg");
			mpGLTextures[i] = LoadJPG(temptexture->strName);
			break;
		case eBMP:
			{
				char	*bmptex = new char[strlen(temptexture->strName + 4)];
				strcpy(bmptex, temptexture->strName);
				strcat(bmptex, ".bmp");
				mpGLTextures[i] = LoadBMP(bmptex);
			break;
			}
		case eSymbolic:
			//FIXME: some kind of signal?>
			break;
		default:
			break;	//FIXME: do anything special here?
		}
		ptr += TEXTURE_SIZE;
	}

	mNumLightMaps = Lumps[kLightmaps].length / LIGHTMAP_SIZE;
	mpLightMaps = new	UINT[mNumLightMaps];
	ptr = Buffer;
	ptr += Lumps[kLightmaps].offset;
	
	for(i = 0; i < mNumLightMaps; i++)
	{
		tBSPLightmap	*tempLightmap = (tBSPLightmap*)ptr;
		mpLightMaps[i] = GenLightmap(tempLightmap); 
		ptr += LIGHTMAP_SIZE;
	}
}

void	BSP::LoadBrushes()
{
	mNumBrushes = Lumps[kBrushes].length / BRUSH_SIZE;
	mNumBrushSides = Lumps[kBrushSides].length / BRUSH_SIDE_SIZE;
	mNumLeafBrushes = Lumps[kLeafBrushes].length / sizeof(int);

	mpBrushes = new tBSPBrush[mNumBrushes];
	mpBrushSides = new tBSPBrushSide[mNumBrushSides];
	mpLeafBrushes = new int[mNumLeafBrushes];

	int i;
	ptr = mpBuffer;
	ptr += Lumps[kLeafBrushes].offset;
	for(i = 0; i < mNumLeafBrushes; i++)
	{
		mpLeafBrushes[i] = *(int*)ptr;
		ptr += sizeof(int);
	}

	ptr = mpBuffer;
	ptr += Lumps[kBrushes].offset;

	for(i = 0; i < mNumBrushes; i++)
	{
		tBSPBrush	*tempBrush = (tBSPBrush*)ptr;
		memcpy(&mpBrushes[i], tempBrush, BRUSH_SIZE);
		
		ptr += BRUSH_SIZE;
	}
	
	ptr = mpBuffer;
	ptr += Lumps[kBrushSides].offset;

	for(i = 0; i < mNumBrushSides; i++)
	{
		tBSPBrushSide	*tempBrushSide = (tBSPBrushSide*)ptr;
		memcpy(&mpBrushSides[i], tempBrushSide, BRUSH_SIDE_SIZE);
		
		ptr += BRUSH_SIDE_SIZE;
	}
}

float		BSP::Trace(Vector3 *start, Vector3 *end, float b)
{
	mBSPColInfo.closestFraction = 1.0f;
	mBSPColInfo.BSPStart = *start;
	mBSPColInfo.BSPEnd = *end;
	int	shared	=	FindAncestor(start, end, b);
	if(shared<0) //they are in the same leaf
	{
		CheckLeafForCollision(-(shared+1), start, end, b);
		return	mBSPColInfo.closestFraction;
	}
	else
	{
		CheckNode(0, 0.0f, 1.0f, &mBSPColInfo.BSPStart, &mBSPColInfo.BSPEnd,b);
		return	mBSPColInfo.closestFraction;
	}
}


//Seems to work with sphere taken into account
int	BSP::FindAncestor(Vector3	*Start, Vector3	*End, float RADIUS)
{
	float	startdist, enddist;
	int	i = 0;
	while(i >= 0)
	{
		tBSPNode	*tempNode = &mpNodes[i];
		tBSPPlane	*tempPlane = &mpPlanes[tempNode->plane];
		
		startdist = DotProduct(Start, &tempPlane->vNormal) - tempPlane->d;
		enddist   = DotProduct(End, &tempPlane->vNormal) - tempPlane->d;

		if(startdist >= RADIUS && enddist >= RADIUS)
		{
			i = tempNode->front;
		}
		else	if(startdist < -RADIUS && enddist < -RADIUS)
		{
			i = tempNode->back;
		}
		else	if( (startdist >= RADIUS && enddist < -RADIUS) || (startdist < -RADIUS && enddist >= RADIUS) )
		{
			return	i; //Node index of ancestor because it spans the plane 
		}
		else
		{
			return	i; //too close, gotta stop 
		}
		
	}

	return i; //sameleaf
}

void	BSP::CheckLeafForCollision(int LeafIndex, Vector3 *Start, Vector3 *End, float b)
{
int	i;
	tBSPLeaf	*tempLeaf = &mpLeafs[LeafIndex];
	for(i = 0; i < tempLeaf->numOfLeafBrushes; i++)
	{
		tBSPBrush	*tempBrush = &mpBrushes[mpLeafBrushes[tempLeaf->leafBrush+i]];
		if(mpTextures[tempBrush->textureID].contents & 1)
			CheckBrush(tempBrush,b);
	}
}


void	BSP::CheckBrush(tBSPBrush* pBrush, float b)
{
	float	startfraction = -1.0f;
	float	endfraction = 1.0f;
	float	startdist, enddist;
	startdist = enddist = 0.0f;
	int	i, normal;
	
	for(i = 0; i < pBrush->numOfBrushSides; i++)
	{
		tBSPBrushSide	*tempBrushSide = &mpBrushSides[pBrush->brushSide+i];
		tBSPPlane		*tempPlane	   = &mpPlanes[tempBrushSide->plane];
		
		startdist = DotProduct(&mBSPColInfo.BSPStart, &tempPlane->vNormal) - tempPlane->d - b;
		enddist   = DotProduct(&mBSPColInfo.BSPEnd, &tempPlane->vNormal) - tempPlane->d - b;
		
		if(startdist >= 0 && enddist >= 0)
		{
			return;	//won't span this brush so fuck it
		}
		
		if(startdist < 0 && enddist < 0)
		{
			continue;	//both behind the plane so may get clipped by another
		}

				float	fraction;
				//normal = tempBrushSide->plane;
				if(startdist > enddist)
				{
					fraction = (startdist - EPSILON) * (1/(startdist-enddist));
					if(startfraction < fraction)
					{
						startfraction = fraction;
						normal = tempBrushSide->plane;
					}
				}
				else	if(startdist < enddist)
				{
					fraction = (startdist + EPSILON) * (1/(startdist-enddist));
					if(endfraction > fraction)
						endfraction = fraction;
				}
	}
	
	if(startfraction < endfraction)
	{
		if(startfraction > -1 && startfraction < mBSPColInfo.closestFraction)
		{
				if(startfraction < 0)
					startfraction = 0;
			mBSPColInfo.closestFraction = startfraction;
			mBSPColInfo.hitNormal = mpPlanes[normal].vNormal;
		}
	}
}

void	BSP::CheckNode(int nodeIndex, float startFraction, float endFraction, Vector3*Start, Vector3*End, float b)
{
	if(mBSPColInfo.closestFraction <= startFraction)
	{
		return;
	}
	if(nodeIndex < 0)
	{
		CheckLeafForCollision(-(nodeIndex + 1), &mBSPColInfo.BSPStart, &mBSPColInfo.BSPEnd, b);
		return;
	}

	float	startdist;
	float	enddist;
	tBSPNode	*tempNode = &mpNodes[nodeIndex];
	tBSPPlane	*tempPlane = &mpPlanes[tempNode->plane];

	startdist = DotProduct(Start, &tempPlane->vNormal) - tempPlane->d;
	enddist   = DotProduct(End, &tempPlane->vNormal) - tempPlane->d;
	
	if(startdist >= b && enddist >= b)
	{
		CheckNode(tempNode->front, startFraction, endFraction, Start, End, b);
	}
	else	if(startdist < -b && enddist < -b)
	{
		CheckNode(tempNode->back, startFraction, endFraction, Start, End, b);
	}
	else
	{
		//If they span the plane the absolutes WILL work

		int	side1, side2;
		float	fraction1, fraction2;
		Vector3	middle;
		float	totaldist = absolute(startdist) + absolute(enddist);
		//this makes sure it spans the plane
		if( (startdist>=0&&startdist-b>=0) && (enddist<0&&enddist+b<0) )
		{
			side1 = tempNode->front;
			side2 = tempNode->back;
			fraction1 = (startdist + EPSILON + b) * (1/totaldist);
			fraction2 = (startdist - EPSILON - b) * (1/totaldist);
		}
		else	if((startdist<0&&startdist+b<0) && (enddist>=0&&enddist-b>=0))
		{
			side1 = tempNode->back;
			side2 = tempNode->front;
			float	absstart = absolute(startdist);
			fraction1 = (absstart - EPSILON - b) * (1/totaldist);
			fraction2 = (absstart + EPSILON + b) * (1/totaldist);
		}
		else //An odd case, you don't span the plane, but the edge of your bounding sphere can cross into the
		{		//adjacent leaf, so you must check that leaf for collisions 
			side1 = tempNode->front;
			side2 = tempNode->back;
			fraction1 = 1.0f;
			fraction2 = 0.0f;
		}
		
	
		if(fraction1 < 0)
			fraction1 = 0;
		if(fraction1 > 1)
			fraction1 = 1;

		if(fraction2 < 0)
			fraction2 = 0;
		if(fraction2 > 1)
			fraction2 = 1;
		
		middle = *Start + ((*End - *Start) * fraction1);
		CheckNode(side1, startFraction, fraction1, Start, &middle, b);

		middle = *Start + ((*End - *Start) * fraction2);
		CheckNode(side2, fraction2, endFraction, &middle, End, b);
	}
}
