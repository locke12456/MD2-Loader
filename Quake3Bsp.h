#ifndef _QUAKE3BSP_H
#define _QUAKE3BSP_H

#include "Vector.h"
#include <vector>

// This is the number that is associated with a face that is of type "polygon"
#define FACE_POLYGON	1
#define MAX_PLANES_CROSSED 100 //FIXME: don't know what to base this value off of
#define EPSILON .03125 //1/32, famous distance from plane when checking stuff 
//structure size definitions, in case I need to change the structures
#define VERTEX_SIZE 44
#define FACE_SIZE 104
#define TEXTURE_SIZE 72
#define LIGHTMAP_SIZE 49152 
#define NODE_SIZE 36
#define LEAF_SIZE 48
#define PLANE_SIZE 16
#define VISDATA_SIZE 12
#define BRUSH_SIZE 12
#define BRUSH_SIDE_SIZE 8
#define SHADER_SIZE	72	//haven't actually checked this but it is the exact same as TEXTURE_SIZE


// This is our integer vector structure
struct tVector3i
{
	int x, y, z;				// The x y and z position of our integer vector
};


// This is our BSP header structure
struct tBSPHeader
{
    char strID[4];				// This should always be 'IBSP'
    int version;				// This should be 0x2e for Quake 3 files
}; 


// This is our BSP lump structure
struct tBSPLump
{
	int offset;					// The offset into the file for the start of this lump
	int length;					// The length in bytes for this lump
};


// This is our BSP vertex structure
struct tBSPVertex
{
    Vector3 vPosition;			// (x, y, z) position. 
    Vector2 vTextureCoord;		// (u, v) texture coordinate
    Vector2 vLightmapCoord;		// (u, v) lightmap coordinate
    Vector3 vNormal;			// (x, y, z) normal vector
    byte color[4];				// RGBA color for the vertex 
};


// This is our BSP face structure
struct tBSPFace
{
    int textureID;				// The index into the texture array 
    int effect;					// The index for the effects (or -1 = n/a) 
    int type;					// 1=polygon, 2=patch, 3=mesh, 4=billboard 
    int startVertIndex;			// The starting index into this face's first vertex 
    int numOfVerts;				// The number of vertices for this face 
    int meshVertIndex;			// The index into the first meshvertex 
    int numMeshVerts;			// The number of mesh vertices 
    int lightmapID;				// The texture index for the lightmap 
    int lMapCorner[2];			// The face's lightmap corner in the image 
    int lMapSize[2];			// The size of the lightmap section 
    Vector3 lMapPos;			// The 3D origin of lightmap. 
    Vector3 lMapVecs[2];		// The 3D space for s and t unit vectors. 
    Vector3 vNormal;			// The face normal. 
    int size[2];				// The bezier patch dimensions. 
	int		LastFrameDrawn;	//lastframe the face was drawn on so you don't draw it more than once 
};


// This is our BSP texture structure
struct tBSPTexture
{
    char strName[64];			// The name of the texture w/o the extension 
    int flags;					
    int contents;				
};

// This is our BSP lightmap structure which stores the 128x128 RGB values
struct tBSPLightmap
{
    byte imageBits[128][128][3];   // The RGB data in a 128x128 image
};


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This stores a node in the BSP tree
struct tBSPNode
{
    int plane;					// The index into the planes array 
	int front;					// The child index for the front node 
	int back;					// The child index for the back node 
	int parent;					// the node's parent node
    tVector3i min;				// The bounding box min position. 
    tVector3i max;				// The bounding box max position. 
}; 

// This stores a leaf (end node) in the BSP tree
struct tBSPLeaf
{
    int cluster;				// The visibility cluster 
    int area;					// The area portal 
    tVector3i min;				// The bounding box min position 
    tVector3i max;				// The bounding box max position 
    int leafface;				// The first index into the face array 
    int numOfLeafFaces;			// The number of faces for this leaf 
    int leafBrush;				// The first index for into the brushes 
    int numOfLeafBrushes;		// The number of brushes for this leaf 
	bool valid;					// True if this leaf cannot see all other leaves 
	int	parent;
}; 

// This stores a splitter plane in the BSP tree
struct tBSPPlane
{
    Vector3 vNormal;			// Plane normal. 
    float d;					// The plane distance from origin 
};

// This stores the cluster data for the PVS's
struct tBSPVisData
{
	int numOfClusters;			// The number of clusters
	int bytesPerCluster;		// The amount of bytes (8 bits) in the cluster's bitset
	byte *pBitsets;				// The array of bytes that holds the cluster bitsets				
};

struct tBSPShader
{
    char strName[64];     // The name of the shader file 
    int brushIndex;       // The brush index for this shader 
    int unknown;          // This is 99% of the time 5
}; 
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is our lumps enumeration
enum eLumps
{
    kEntities = 0,				// Stores player/object positions, etc...
    kTextures,					// Stores texture information
    kPlanes,				    // Stores the splitting planes
    kNodes,						// Stores the BSP nodes
    kLeafs,						// Stores the leafs of the nodes
    kLeafFaces,					// Stores the leaf's indices into the faces
    kLeafBrushes,				// Stores the leaf's indices into the brushes
    kModels,					// Stores the info of world models
    kBrushes,					// Stores the brushes info (for collision)
    kBrushSides,				// Stores the brush surfaces info
    kVertices,					// Stores the level vertices
    kMeshVerts,					// Stores the model vertices offsets
    kShaders,					// Stores the shader files (blending, anims..)
    kFaces,						// Stores the faces for the level
    kLightmaps,					// Stores the lightmaps for the level
    kLightVolumes,				// Stores extra world lighting information
    kVisData,					// Stores PVS and cluster info (visibility)
    kMaxLumps					// A constant to store the number of lumps
};

struct tBSPBrush 
{
    int brushSide;           // The starting brush side for the brush 
    int numOfBrushSides;     // Number of brush sides for the brush
    int textureID;           // The texture index for the brush
}; 

struct tBSPBrushSide 
{
    int plane;              // The plane index
    int textureID;          // The texture index
}; 

enum	eEndType
{
	eSameLeaf = 0,
	eDifferentLeaf,
	eInvalidLeaf //outside the world 
};

struct segment
{
	segment(){}
	segment(float	per)
	{
		Percent = per;	
	}
	int		LeafIndex;
	int		PlaneIndex; 
	float	Percent;
};

struct	CollisionInfo
{
	Vector3		BSPStart;
	Vector3		BSPEnd;
	Vector3		hitNormal;
	float		closestFraction;
	float		hackFraction;
};
//What you ultimately want is a trace function that defines the entire move
class	BSP
{
public:

	//Methods 
	BSP() 
	{ //just give it some data 
	mBSPColInfo.closestFraction = 1.0f;
	mBSPColInfo.hitNormal = Vector3(0, 0, 0);
	mBSPColInfo.BSPStart = Vector3(0, 0, 0);
	mBSPColInfo.hackFraction = 0.0f;
	} 
	void	TouchTexture(char*);//don't ask 
	void	MakeYUp(Vector3*);
	void	MakeYUp(tVector3i*);
	bool	LoadBSP(char	*filename);
	void	Render(int);
		bool	IsLeafPV(int test, int current); 
		void	RenderLeaf(tBSPLeaf*);
		void	RenderFace(int);
		
	UINT	GenLightmap(tBSPLightmap*);
	int		FindLeaf(Vector3*);
	int		FindLeaf(Vector3*, std::vector<int>*);
	int		FindLeafFromCurrent(Vector3*, int	Current); //same as findleaf except starts at current 
	
	bool	InFront(Vector3*, tBSPPlane*); //determines if a point is in front of behind a plane
	
	void	CalculateParentNodes();
		void	SetMeAsParent(int NodeIndex, int ParentIndex);

	void	CalculateValidLeafs(); //Goes through each leaf and makes sure it cannot see all of the other leaves
	
	void	DrawBBox(tVector3i	min, tVector3i	max);

	void	DeleteTextures(); 
	void	LoadGLTextures(char	*); //Loads OpenGL textures NOT tBSPTextures 
	
	void	LoadTextures();	//Loads tBSPTextures NOT OpenGL textures
	void	LoadBrushes();

	//Collision detection stuff
	float	Trace(Vector3	*Start, Vector3		*End, float);

		void	CheckLeafForCollision(int Leaf, Vector3	*Start, Vector3	*End, float);

		void	CheckBrush(tBSPBrush*,float);
		void	CheckNode(int	node, float	startfrac, float endfrac, Vector3*, Vector3*, float);
		void	BoxCheckNode(int node, float startfrac, float endfrac, Vector3*start, Vector3*end);
		
		int		FindPathIntersect(int, int); 
		bool	SpansPlane(Vector3	*Start, Vector3	*End, int	LeafIndex);
		void	FindPlanesCrossed(int currentstart, Vector3	*start, Vector3	*end, std::vector<segment*>	*crossed); 
		int		FindAncestor(Vector3	*Start, Vector3	*End, float);
	
		CollisionInfo	mBSPColInfo;
		
	/*Collision detection stuff*/
	//testing thing
	void	DumpTree(int); //writes entire tree from certain node to trace file

	tBSPLump	Lumps[kMaxLumps];	
	int			mNumVerts;
	tBSPVertex	*mpVertices;
	
	int			mNumFaces;
	tBSPFace	*mpFaces;

	int			mNumNodes;
	tBSPNode	*mpNodes;

	int			mNumLeafs;
	tBSPLeaf	*mpLeafs;

	int			mNumLeafFaces;
	int			*mpLeafFaces; //leaves index into this array.  The value this array holds are indices into faces
	
	int			mNumPlanes;
	tBSPPlane	*mpPlanes;
	
	tBSPVisData	mVisData; //only one of these suckers, but holds bitsets for entire world
						  //want to build BSP tree off of this

	int			mNumBrushes;
	int			mNumBrushSides;
	int			mNumLeafBrushes;
	tBSPBrush	*mpBrushes;	 //the actual brushes themselves 
	tBSPBrushSide	*mpBrushSides; //one plane of brush
	int			*mpLeafBrushes;	//indexes into brush array from leaves 

	int			mNumShaders;
	tBSPShader	*mpShaders;

	int			mNumTextures;
	tBSPTexture	*mpTextures;	//holds texture ids and stuff 
	
	unsigned int	*mpGLTextures;

	int		mNumLightMaps;
	unsigned int	*mpLightMaps;

	int		filesize;
	byte	*mpBuffer;
	byte	*ptr; 
};



#endif


