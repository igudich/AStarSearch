#pragma once
#include "Node.h"
#include "TPrimitive.h"
#include "Graph.h"
#include <string>

typedef Node<TPrimitive> primNode;
typedef Graph<TPrimitive> primGraph;
class GraphSerializer
{
public:
	void loadFromStream(std::istream stream, primGraph pg);
	GraphSerializer();
	~GraphSerializer();
};

