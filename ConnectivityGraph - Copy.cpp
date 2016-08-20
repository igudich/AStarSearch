#include "ConnectivityGraph.h"
#include "Link.h"
#include <sstream>
#include <fstream>
#include <map>
#include <chrono>

using namespace std;

ConnectivityGraph::ConnectivityGraph() : fictiveId(-1)
{
}


ConnectivityGraph::~ConnectivityGraph()
{
}

float ConnectivityGraph::pathLength(const std::list<int>& path)
{
	return prGr.pathLength(path);
}

void ConnectivityGraph::insertElem(TPrimitive e)
{
	prGr.insertElem(e);
	if (e.component_) //inserting component
		hypGr.insertElem(HyperPrimitive(e.id_, HyperPrimitive::COMPONENT));
	else //inserting element (need to insert in a set) 
		if (!hypGr.contain(e.set_))
			hypGr.insertElem(HyperPrimitive(e.set_, HyperPrimitive::SET));

}

#define PENALTY 1000000000
#define SUPER_PENALTY 10000000000
void ConnectivityGraph::insertLink(const Link& lk)
{
//	prGr.insertLink(lk);
	auto eFromPtr = prGr.getElemPtr(lk.from);
	auto eToPtr = prGr.getElemPtr(lk.to);

	if (eFromPtr->component_ && eToPtr->component_)
	{
		hypGr.insertLink(Link(eFromPtr->id_, eToPtr->id_, PENALTY));
		prGr.insertLink(lk);
	}
	else if (eFromPtr->component_)
	{
		hypGr.insertLink(Link(eFromPtr->id_, eToPtr->set_, PENALTY));
		auto hpPtr = hypGr.getElemPtr(eToPtr->set_);
		if (hpPtr != NULL)
			hpPtr->cnt_++;
		prGr.insertLink(lk);
	}
	else if (eToPtr->component_)
	{
		hypGr.insertLink(Link(eFromPtr->set_, eToPtr->id_, PENALTY));
		auto hpPtr = hypGr.getElemPtr(eFromPtr->set_);
		if (hpPtr != NULL)
			hpPtr->cnt_++;
		prGr.insertLink(lk);
	}
	else if (eFromPtr->set_ != eToPtr->set_)  //link between 2 different sets
											//creating fictive 
	{
		int id = fictiveId;
		--fictiveId;
		TPrimitive pr(-1, -1, id, true, -1);
		insertElem(pr);
		Link lk1(id, lk.from, SUPER_PENALTY);
		Link lk2(id, lk.to, SUPER_PENALTY);
		prGr.insertLink(lk1);
		prGr.insertLink(lk2);

		hypGr.insertLink(Link(eFromPtr->set_, id, SUPER_PENALTY));
		auto hpPtr = hypGr.getElemPtr(eFromPtr->set_);
		if (hpPtr != NULL)
			hpPtr->cnt_++;

		hypGr.insertLink(Link(eToPtr->set_, id, SUPER_PENALTY));
		hpPtr = hypGr.getElemPtr(eToPtr->set_);
		if (hpPtr != NULL)
			hpPtr->cnt_++;

		prGr.insertLink(lk1);
		prGr.insertLink(lk2);
	}
	else
		prGr.insertLink(lk);
	
}

void ConnectivityGraph::removeElement(int id)
{
	TPrimitive* prPtr = prGr.getElemPtr(id);
	if (prPtr->component_)
	{
		hypGr.remove(id);
		prGr.remove(id);
	}
	else
	{
		HyperPrimitive* hypPrPrt = hypGr.getElemPtr(prPtr->set_);
		int hypId = hypPrPrt->id_;

		hypPrPrt->cnt_--;
		if(hypPrPrt->cnt_ == 0)
			hypGr.remove(hypId);
		prGr.remove(id);
	}
		
}

void ConnectivityGraph::insertLinks(const std::list<Link>& lk)
{
	prGr.insertLinks(lk);
	for (auto& it : lk)
		insertLink(it);
}

void printPath(const list<int>& path)
{
	for (auto& it : path)
		cout << it << " --> ";
	cout << endl;
}

std::list<int> ConnectivityGraph::findPath(int start, int goal)
{ //start & goal are not the components but elements in some set
	TPrimitive* sPrim = prGr.getElemPtr(start);
	TPrimitive* gPrim = prGr.getElemPtr(goal);
	if (sPrim->set_ == gPrim->set_)
		return prGr.findPathSameSet(start, goal);
	else
	{
		auto t_start = std::chrono::high_resolution_clock::now();
		list<int> hypPth = hypGr.findPath(gPrim->set_, sPrim->set_);
		auto t_end = std::chrono::high_resolution_clock::now();

		cout << "duration hyper path = " <<
			std::chrono::duration<double, std::milli>(t_end - t_start).count() <<
			" ms." << endl;

		list<int> pth;
		cout << "Hyper path:" << endl;
		printPath(hypPth);
		
		//searching the path from start set element to component
		auto hypPthIt = hypPth.begin();
		hypPthIt++;
		t_start = std::chrono::high_resolution_clock::now();
		list<int> outPth = prGr.findPath(*hypPthIt, start, sPrim->set_);
		t_end = std::chrono::high_resolution_clock::now();

		cout << "duration out path = " <<
			std::chrono::duration<double, std::milli>(t_end - t_start).count() <<
			" ms." << endl;

		cout << "Out path:" << endl;
//		printPath(outPth);
		//loop searching paths from component to component trough the set
		hypPth.pop_front();
		while (hypPth.size() > 2)
		{
			hypPthIt = hypPth.begin();
			int comFrom = *(hypPthIt);
			++hypPthIt;
			int setThrough = *(hypPthIt);
			++hypPthIt;
			int comTo = *(hypPthIt);

			cout << comTo << " <-- " << comFrom << endl;

			t_start = std::chrono::high_resolution_clock::now();
			pth = prGr.findPath(comTo, comFrom, setThrough);
			t_end = std::chrono::high_resolution_clock::now();

			cout << "duration  path = " <<
				std::chrono::duration<double, std::milli>(t_end - t_start).count() <<
				" ms." << endl;
//			printPath(pth);
			
			for (auto& it : pth)
				outPth.push_back(it);
			pth.clear();

			hypPth.pop_front();
			hypPth.pop_front();
		}

		//searching the path from last component to goal set element
		if (!hypPth.empty())
		{
			hypPthIt = hypPth.begin();

			t_start = std::chrono::high_resolution_clock::now();
			pth = prGr.findPath(goal, *hypPthIt, gPrim->set_);
			t_end = std::chrono::high_resolution_clock::now();

			cout << "duration in path = " <<
				std::chrono::duration<double, std::milli>(t_end - t_start).count() <<
				" ms." << endl;
//			printPath(pth);

			if (!pth.empty())
			{
				pth.pop_front();
				for (auto& it : pth)
					outPth.push_back(it);
			}
			else
				cout << "Connectivity corruption" << endl;
		}

		return outPth;
	}

	return std::list<int>();
}

void ConnectivityGraph::read(string filename)
{
	fstream fstr;
	stringstream sstr;
	string s;
	int id, x, y, set, comp;
	int lid;
	map<int, TPrimitive> primitives;
	list<Link> links;
	map<TPrimitive, list<Link> > componentLinks;

	fstr.open(filename);
	while (!fstr.eof())
	{
		getline(fstr, s);

		getline(fstr, s);
		sstr.clear();
		sstr << s;
		sstr >> id >> x >> y >> set >> comp;
		if (comp)
			set = -1;


		TPrimitive primitive(x, y, id, static_cast<bool>(comp), set);
		primitives.insert(pair<int, TPrimitive>(id, primitive));
			

		sstr.clear();
		getline(fstr, s);
		if (!s.empty())
		{
			sstr << s;
			while (!sstr.eof())
			{
				sstr >> lid;
				if (lid != id)
					links.push_back(Link(id, lid, 0.0));
			}
		}
	}

	fstr.close();

//	componentLinks.insert(pair <TPrimitive, list<Link> >(primitive, list<Link>()));

	//set Link weights as euclidian length
	for (auto& it : links)
	{
		auto fromIt = primitives.find(it.from);
		auto toIt = primitives.find(it.to);


		if (fromIt == primitives.end())
			continue;
		if (fromIt == primitives.end())
			continue;

		const TPrimitive& from = fromIt->second;
		const TPrimitive& to = toIt->second;


		float dx = static_cast<float>(from.x_ - to.x_);
		float dy = static_cast<float>(from.y_ - to.y_);

		it.weight = sqrtf(dx*dx + dy*dy);
	}

	auto t_start = std::chrono::high_resolution_clock::now();
	for (auto& it : primitives)
		insertElem(it.second);


	for (auto& it : links)
		insertLink(it);
	auto t_end = std::chrono::high_resolution_clock::now();
	
	cout << "duration of load = " <<
		std::chrono::duration<double, std::milli>(t_end - t_start).count() <<
		" ms." << endl;

//	prGr.print();
//	prGr.toGraphize("D:/dev/graph.txt");
//	cout << endl << endl;

	hypGr.toGraphize("./hyp_graph.txt");
//	hypGr.print();
//	cout << endl << endl;
}