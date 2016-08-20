#pragma once
#include <map>
#include <iostream>
#include <vector>
#include <iostream>

template<class Elem>
class Node
{
public:
	Elem elem; //some content
	std::vector<std::pair<float, Node<Elem>* > > succ; //(weight, node link)

	void print()
	{
		std::cout << elem.id_ << " : ";
		for (auto& it : succ)
			std::cout << it.second->elem.id_ << ", ";

		std::cout << std::endl;
	}

	Node<Elem>* getSuccAddWeight(int id, float& w)
	{
		for (auto& it : succ)
			if (it.second->elem.id_ == id)
			{
				w += it.first;
				return it.second;
			}

			std::cout << "id = " << id << std::endl;
		return NULL;
	}

	void removeLink(int idx);

	Node(Elem e) : elem(e)
	{}
};

template<class Elem>
void Node<Elem>::removeLink(int id)
{
	for (auto it = succ.begin(); it != succ.end(); ++it)
	if (it->second->elem.id_ == id)
	{
		succ.erase(it);
		break;
	}
				
}