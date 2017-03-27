#ifndef CAGES_H
#define CAGES_H
#include <iostream>
#include <limits>
using namespace std;

class Cages {
	bool visited;
	double distV;
	int prevV;
	int xCoord;
	int yCoord;
	char description;
public:
	Cages() : visited(false), distV(numeric_limits<double>::infinity()) {};
	Cages(int x, int y, char d) : visited(false), distV(numeric_limits<double>::infinity())
			, xCoord(x), yCoord(y), description(d) {}; 
	void setVisited() {
		visited = true;
	}
	void setPrevV(int cageNum) {
		prevV = cageNum;
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
	int getPrevV() {
		return prevV;
	}
	int getXCoord() {
		return xCoord;
	}
	int getYCoord() {
		return yCoord;
	}
	char getDescription() {
		return description;
	}
};

#endif
