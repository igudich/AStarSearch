#pragma once
#include <list>
#include <map>


struct searchAttr
{
	float h, g;
	int came;

	searchAttr(float _h, float _g, int c) : h(_h), g(_h), came(c){}
};

std::list<int> buildPath(std::map<int, searchAttr>& attrMap, int current);
