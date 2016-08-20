#pragma once

class TPrimitive
{
public:
	int id_;
	int x_;
	int y_;
	int set_;
	bool component_;
public:
	TPrimitive(int x = -1, int y = -1, int id = -1, bool component = false, int set = -1);
	bool operator == (const TPrimitive &p) { return x_ == p.x_ && y_ == p.y_; }
	~TPrimitive();
};




