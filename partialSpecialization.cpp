#pragma once
#include "Graph.h"
#include <ctime>

template<>
bool inOtherSet<TPrimitive>(const TPrimitive& base, const TPrimitive& neib)
{
	return(base.set_ != neib.set_ || neib.component_);
}


template<>
bool inSet<TPrimitive>(const TPrimitive& e, int set)
{
	return e.set_ == set;
}

template<>
bool isComponent<TPrimitive>(const TPrimitive& e)
{
	return e.component_;
}

template<>
float Graph<TPrimitive>::euclidianHeuristic(int start, int goal)
{
	TPrimitive* sPtr = getElemPtr(start);
	TPrimitive* gPtr = getElemPtr(goal);
	if (sPtr != NULL && gPtr != NULL)
	{
		int dx = sPtr->x_ - gPtr->x_;
		int dy = sPtr->y_ - gPtr->y_;
		return sqrtf(static_cast<float>(dx*dx + dy*dy));
	}
	return -1.0;	
}