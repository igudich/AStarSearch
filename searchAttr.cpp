#include "searchAttr.h"

std::list<int> buildPath(std::map<int, searchAttr>& attrMap, int current)
{
	std::list<int> totalPath;
	while (current != std::numeric_limits<int>::min())
	{
		totalPath.push_back(current);
		current = attrMap.find(current)->second.came;
	}

	return totalPath;
}
