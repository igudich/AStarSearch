#pragma once
#include "Node.h"
#include <cmath>
#include <list>
#include <set>
#include <limits>
#include <algorithm>
#include <fstream>
#include "TPrimitive.h"
#include "searchAttr.h"

class Link;


template<class Elem>
class Graph
{
	std::map<int, Node<Elem>* > g; //storage for graph nodes
protected:
	float euclidianHeuristic(int start, int goal);
	std::list<int> reconstrucPath(int current);
public:
	void clean();
	void insertElem(Elem e); //inserts element
	void insertLinks(const std::list<Link>& lk); //insert links
	bool insertLink(const Link& lk); //true if successgully inserted
	void remove(int id); //removes elements with all links
	
	//finds shortest path from head to tail (A*)
	std::list<int> findPath(int start, int goal); 
	
	//finds shortest path from head to tail (A*) for path from component
	// to component trough the given set
	std::list<int> findPath(int start, int goal, int st); 
	
	
	std::list<int> findPathSameSet(int start, int goal); //finds shortest path from head to tail (A*)
	
	
	void print();
	bool contain(int id);
	Elem* getElemPtr(int id);
	float pathLength(const std::list<int>& path);
	int findId(const Elem& e);

	void toGraphize(std::string fname);
	
	Graph();
	~Graph();
};

template<class Elem>
Graph<Elem>::Graph()
{
}

template<class Elem>
Graph<Elem>::~Graph()
{
	clean();
}

template<class Elem>
void Graph<Elem>::clean()
{
	for (auto& it : g)
		delete it.second;
}

template<class Elem>
void Graph<Elem>::toGraphize(std::string fname)
{
	std::ofstream ofst;
	ofst.open(fname);
	ofst << "graph sch {" << std::endl;
	for (auto& it : g)
		ofst << it.first << ";" << std::endl;

	for (auto& it : g)
	{
		std::vector<std::pair<float, Node<Elem>* > >& sc = it.second->succ;
		for (auto& k : sc)
		if (it.first < k.second->elem.id_)
				ofst << it.first << " -- " << k.second->elem.id_ << ";" << std::endl;
	}

	ofst << "}";
	ofst.close();
}

template<class Elem>
int Graph<Elem>::findId(const Elem& e)
{
	for (auto& it : g)
	if (it.second->elem == e)
		return it.first;
	return std::numeric_limits<int>::infinity();
}

template<class Elem>
float Graph<Elem>::pathLength(const std::list<int>& path)
{
	if (path.empty())
		return 0;
	float len = 0;
	auto pathIt = path.begin();
	auto gIt = g.find(*pathIt);
	if (gIt == g.end())
		return -1; //no node 
	Node<Elem>* ndPtr = gIt->second;
	Node<Elem>* ndPtrBuf = NULL;
	++pathIt;
	while (pathIt != path.end())
	{
		int nextId = *pathIt;

		ndPtrBuf = ndPtr->getSuccAddWeight(nextId, len);
		if (ndPtrBuf == NULL)
			return -2; // no neighbor 

		ndPtr = ndPtrBuf;
		++pathIt;
	}
	return len;
}

template<class Elem>
Elem* Graph<Elem>::getElemPtr(int id)
{
	auto it = g.find(id);
	Elem* res = NULL;
	if(it != g.end()) 
		res = &(it->second->elem);
	return res;
}

template<class Elem>
bool Graph<Elem>::contain(int id)
{
	return g.find(id) != g.end();
}

template<class Elem>
float Graph<Elem>::euclidianHeuristic(int start, int goal)
{
	return 0;
}

template<>
float Graph<TPrimitive>::euclidianHeuristic(int start, int goal);

template<class Elem>
void Graph<Elem>::print()
{
	for (auto& it : g)
		it.second->print();
}

template<class Elem>
void Graph<Elem>::insertElem(Elem e)
{
	g.insert(std::pair<int, Node<Elem>*> (e.id_, new Node<Elem>(e)));
}

template<class Elem>
bool Graph<Elem>::insertLink(const Link& lk)
{
	Node<Elem>* npFrom = g.find(lk.from)->second;
	Node<Elem>* npTo = g.find(lk.to)->second;
	//check if link already exists
	bool b = false;
	for (auto& k : npFrom->succ)
	if (k.second->elem.id_ == lk.to)
	{
		b = true;
		break;
	}
	//add link to graph both verticies
	if (!b)
	{
		npFrom->succ.push_back(std::pair<float, Node<Elem>* >(lk.weight, npTo));
		npTo->succ.push_back(std::pair<float, Node<Elem>* >(lk.weight, npFrom));
	}
	return !b;
}


template<class Elem>
void Graph<Elem>::insertLinks(const std::list<Link>& lk)
{
	for (auto& it : lk)
		insertLink(it);
}

template<class Elem>
void Graph<Elem>::remove(int id)
{
	auto pt = g.find(id);
	if (pt != g.end())
	{
		//remove links
		for (auto it = pt->second->succ.begin(); it != pt->second->succ.end(); ++it)
		{
			Node<Elem>* np = it->second;
			np->removeLink(id);
		}
		delete pt->second;
		g.erase(pt);
	}


}

template<class Elem>
struct fcmp // functor to compare according heuristic measure value
{
	std::map<int, searchAttr>& scoreMap;
	fcmp(std::map<int, searchAttr>& sm) : scoreMap(sm) {}
	bool operator() (const int &a, const int &b) const
	{
		return scoreMap.find(a)->second.h < scoreMap.find(b)->second.h;
	}
};

template<class Elem>
bool inOtherSet(const Elem& base, const Elem& neib)
{
	return false;
}

template<>
bool inOtherSet<TPrimitive>(const TPrimitive& base, const TPrimitive& neib);


template<class Elem>
bool inSet(const Elem& e, int set)
{
	return true;
}

template<>
bool inSet<TPrimitive>(const TPrimitive& e, int set);

template<class Elem>
bool isComponent(const Elem& e)
{
	return true;
}

template<>
bool isComponent<TPrimitive>(const TPrimitive& e);

template<class Elem>
std::list<int> Graph<Elem>::findPath(int start, int goal)
{
	auto startIt = g.find(start);
	auto goalIt = g.find(goal);

	std::set<int> closed;
	std::set<int> open;
	open.insert(start);

	std::map<int, searchAttr> attrMap;

	attrMap.insert(std::pair<int, searchAttr>(start,
		searchAttr(euclidianHeuristic(start, goal),
		0.0, std::numeric_limits<int>::min())));
	
	while (!open.empty())
	{
		int current = *(std::min_element(open.begin(), open.end(), fcmp<Elem>(attrMap)));
		if (current == goal)
			return buildPath(attrMap, current);
		else
		{
			open.erase(current);
			closed.insert(current);
			auto currIt = g.find(current);
			auto currMapIt = attrMap.find(current);
			auto& neighbors = currIt->second->succ;
			for (auto& it : neighbors)
			{
				int neib = it.second->elem.id_;
				auto neibIt = g.find(neib);

				auto neibAttrMapIt = attrMap.find(neib);
				if (neibAttrMapIt == attrMap.end())
					neibAttrMapIt = attrMap.insert(std::pair<int, searchAttr>(neib,
					searchAttr(std::numeric_limits<float>::infinity(),
					std::numeric_limits<float>::infinity(),
					std::numeric_limits<int>::min()))).first;

				if (closed.find(neib) != closed.end())
					continue;

				float tentativeGScore = currMapIt->second.g + it.first;

				if (open.find(neib) == open.end())
					open.insert(neib);
				else if (tentativeGScore >= neibAttrMapIt->second.g)
					continue;
				
				neibAttrMapIt->second.came = current;
				neibAttrMapIt->second.g = tentativeGScore;
				neibAttrMapIt->second.h = tentativeGScore + euclidianHeuristic(neib, goal);
			}
		}
	}
	return std::list<int>(0);
}


template<class Elem>
std::list<int> Graph<Elem>::findPath(int startComp, int goal, int st)
{
	auto startIt = g.find(startComp);
	auto goalIt = g.find(goal);

	std::set<int> closed;
	std::set<int> open;
	open.insert(startComp);

	std::map<int, searchAttr> attrMap;

	attrMap.insert(std::pair<int, searchAttr>(startComp,
		searchAttr(euclidianHeuristic(startComp, goal),
		0.0, std::numeric_limits<int>::min())));


	while (!open.empty())
	{
		int current = *(std::min_element(open.begin(), open.end(), fcmp<Elem>(attrMap)));
		if (current == goal)
			return buildPath(attrMap, current);
		else
		{
			open.erase(current);
			closed.insert(current);
			auto currIt = g.find(current);
			auto currMapIt = attrMap.find(current);
			auto& neighbors = currIt->second->succ;
			for (auto& it : neighbors)
			{
				int neib = it.second->elem.id_;
				auto neibIt = g.find(neib);
				
				auto neibAttrMapIt = attrMap.find(neib);
				if (neibAttrMapIt == attrMap.end())
					neibAttrMapIt = attrMap.insert(std::pair<int, searchAttr>(neib,
					searchAttr(std::numeric_limits<float>::infinity(),
					std::numeric_limits<float>::infinity(),
					std::numeric_limits<int>::min()))).first;
				
				if (closed.find(neib) != closed.end())
					continue;
				//only neighbors in given set and components
				if (!(inSet<Elem>(it.second->elem, st) || isComponent<Elem>(it.second->elem))) 
					continue;
			
				float tentativeGScore = currMapIt->second.g + it.first;

				if (open.find(neib) == open.end())
					open.insert(neib);
				else if (tentativeGScore >= neibAttrMapIt->second.g)
					continue;

				

				neibAttrMapIt->second.came = current;
				neibAttrMapIt->second.g = tentativeGScore;
				neibAttrMapIt->second.h = tentativeGScore + euclidianHeuristic(neib, goal);
			}
		}
	}
	return std::list<int>(0);
}


template<class Elem>
std::list<int> Graph<Elem>::findPathSameSet(int start, int goal)
{
	auto startIt = g.find(start);
	auto goalIt = g.find(goal);

	std::set<int> closed;
	std::set<int> open;
	open.insert(start);

	std::map<int, searchAttr> attrMap;

	attrMap.insert(std::pair<int, searchAttr>(start,
		searchAttr(euclidianHeuristic(start, goal),
		0.0, std::numeric_limits<int>::min())));


	while (!open.empty())
	{
		int current = *(std::min_element(open.begin(), open.end(), fcmp<Elem>(attrMap)));
		if (current == goal)
			return buildPath(attrMap, current);
		else
		{
			open.erase(current);
			closed.insert(current);
			auto currIt = g.find(current);
			auto currMapIt = attrMap.find(current);
			auto& neighbors = currIt->second->succ;
			for (auto& it : neighbors)
			{
				int neib = it.second->elem.id_;
				auto neibIt = g.find(neib);

				auto neibAttrMapIt = attrMap.find(neib);
				if (neibAttrMapIt == attrMap.end())
					neibAttrMapIt = attrMap.insert(std::pair<int, searchAttr>(neib,
					searchAttr(std::numeric_limits<float>::infinity(),
					std::numeric_limits<float>::infinity(),
					std::numeric_limits<int>::min()))).first;

				if (closed.find(neib) != closed.end())
					continue;
				if (inOtherSet<Elem>(startIt->second->elem, it.second->elem))
					continue;

				float tentativeGScore = currMapIt->second.g + it.first;

				if (open.find(neib) == open.end())
					open.insert(neib);
				else if (tentativeGScore >= neibAttrMapIt->second.g)
					continue;

				

				neibAttrMapIt->second.came = current;
				neibAttrMapIt->second.g = tentativeGScore;
				neibAttrMapIt->second.h = tentativeGScore + euclidianHeuristic(neib, goal);
			}
		}
	}
	return std::list<int>(0);
}

