#include "TPrimitive.h"


TPrimitive::TPrimitive(int x, int y, int id, bool component, int set) : x_(x),
					 y_(y), 
					 id_(id), 
					 component_(component), 
					 set_(set)
{
}


TPrimitive::~TPrimitive()
{
}
