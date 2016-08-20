#include <iostream>
#include "Graph.h"
#include "TPrimitive.h"
#include "Link.h"
#include "ConnectivityGraph.h"
#include <string>
#include <fstream>
#include <chrono>
#include <windows.h>


using namespace std;

int strigHash(const std::string& s)
{
	int hash = 0;
	int offset = 'a' - 1;
	for (std::string::const_iterator it = s.begin(); it != s.end(); ++it) 
	{
		hash = hash << 1 | (*it - offset);
	}
	return hash;
}




int main()
{
	string fname("C:/Users/gosha/work/SMAforAltium/SIMPLE2.CONN");
//	string fname("D:/dev/KYUCERA.CONN");

	ConnectivityGraph cGr;
	cGr.read(fname);
	ofstream ofstr;

	int from ,to;

	while (1)
	{
		cout << "Enter id's __  __:" << endl;
		cin >> from >> to;
		ofstr.open("C:/Users/gosha/work/SMAforAltium/out.txt");
		auto t_start = std::chrono::high_resolution_clock::now();
		LARGE_INTEGER freq, start, end;
		QueryPerformanceFrequency(&freq);
		//	std::list<int> pth = cGr.findPath(915, 290);
		QueryPerformanceCounter(&start);
		
		std::list<int> pth = cGr.findPath(from, to);
		
		QueryPerformanceCounter(&end);
		auto t_end = std::chrono::high_resolution_clock::now();

		ofstr << "elapsed: " << ((double)end.QuadPart - start.QuadPart) / freq.QuadPart << endl;

		ofstr << "duration = " <<
			std::chrono::duration<double, std::milli>(t_end - t_start).count() <<
			" ms." << endl;

		ofstr << endl << endl << "Final path : " << endl;
		for (auto& it : pth)
			ofstr << it << "; " << std::endl;

		//	cout << endl << endl;
		ofstr << "path len = " << cGr.pathLength(pth) << endl;
		ofstr.close();
	}
	
	int a;
	std::cin >> a;
}