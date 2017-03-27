#ifndef CANAL_H
#define CANAL_H
#include <iostream>
#include <limits>
using namespace std;

class Canal {
	bool visited = false;
	double distV; 
	int xCoord;
	int yCoord;
public:
	Canal() : distV(numeric_limits<double>::infinity()) {};
	Canal(int x, int y) : distV(numeric_limits<double>::infinity())
							, xCoord(x), yCoord(y) {}; 
	void setVisited() {
		visited = true;
	}
	void setDistV(double currentDist) {
		distV = currentDist;
	}
	bool getVisited() {
		return visited;
	}
	double getDistV() {
		return distV;
	}
	int getXCoord() {
		return xCoord;
	}
	int getYCoord() {
		return yCoord;
	}
};

#endif
