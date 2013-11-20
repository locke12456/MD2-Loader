#include "Defines.h"
#include "Quake3BSP.h"
#include "Vector.h"

class	Line
{
public:
	Line(){}

	Vector3	Start;
	Vector3	End;
	float	dist1; 
	float	dist2;

	//Cut calculates a new line segment 

	void	Cut(int PlaneIndex); 
	std::vector<segment*>	Segments; 
	Line	*mpNewLine; 
};