#pragma once
class Link
{
public:
	int from;
	int to;
	float weight;

	Link(int f, int t, float w) : from(f), to(t), weight(w)
	{}
};

