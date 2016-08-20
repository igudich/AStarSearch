#pragma once
#include <set>
class HyperPrimitive
{
public:
	enum type 
	{
		SET,
		COMPONENT,
		FICTIVE_COMPONENT,
		NONE
	};

	int id_;
	type tp_;

	int cnt_; //not empty only for SET, 
	//contains the elements linked to any component 
public:
	HyperPrimitive(int id = -1, type tp = NONE, int n = 0);
	bool operator == (const HyperPrimitive &p) { return id_ == p.id_ && tp_ == p.tp_; }
	~HyperPrimitive();
};

