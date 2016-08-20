#pragma once
#include "Graph.h"
#include "TPrimitive.h"
#include "HyperPrimitive.h"
#include <string>

class ConnectivityGraph
{
	int fictiveId; //negative int
	Graph<TPrimitive> prGr;
	Graph<HyperPrimitive> hypGr; //sets & components graph (set_ uses like id_ for set)
public:
	ConnectivityGraph();
	~ConnectivityGraph();

public:
	float pathLength(const std::list<int>& path);
	void insertElem(TPrimitive e);
	void insertLinks(const std::list<Link>& lk);
	void insertLink(const Link& lk);
	void removeElement(int id);
	std::list<int> findPath(int start, int goal); //find the shortest path from start to goal
	void read(std::string filename);
};

