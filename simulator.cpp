#include <iostream>
#include <getopt.h>
#include <cstring>
#include <iomanip>
#include "cages.h"
#include "canal.h"
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <limits>
#include <deque>
using namespace std;


vector<int> optimalPath;
double currPathDistance = 0;

char getMode(int argc, char *argv[]) {
    int options;
    char zooMode = 'n';
    while (1)
    {
        static struct option long_options[] =
        {
            {"help", no_argument, 0, 'h'},
            {"mode", required_argument, 0, 'm'}
            
        };
        int option_index = 0;
        options = getopt_long (argc, argv, "hm:",
                               long_options, &option_index);
       
        if (options == -1)
            break;
        
        switch (options)
        {
            case 'h':
            	cerr << "The function of this program is to determine "
            		<< "the most efficient routes for providing food "
            		<< "and water to the animals in the zoo." << '\n';
            	exit(0);

            case 'm':
                if ((strcmp(optarg, "MST") != 0) && 
                    (strcmp(optarg, "OPTTSP") != 0) &&
                	(strcmp(optarg, "FASTTSP")) != 0) {
                    cerr << "Invalid modes" << '\n';
                	exit(1);
                }
                else {
                	switch(*optarg) {
                        case 'M':
                            zooMode = 'M';
                            break;
                        case 'O':
                            zooMode = 'O';
                            break;
                        case 'F':
                            zooMode = 'F';
                            break;
                        default:
                            break;
                    }
                }
                break;
            default:
                break;
        }
    }    
  	return zooMode;
}
void createZooMap(vector<Cages> &map) {
	int numCages = 0;
	int x = 0;
	int y = 0;
	cin >> numCages;

	map.reserve(static_cast<size_t>(numCages));
	while(cin >> x >> y) {
		if ((x > 0) || ((x <= 0) && (y > 0))) {
			map.push_back(Cages(x,y,'z'));
		}
		else if ((x < 0) && (y < 0)) {
			map.push_back(Cages(x,y,'w'));
		} 
		else if (((x <= 0) && (y == 0)) || ((x == 0) && (y <= 0))){
			map.push_back(Cages(x,y,'b'));
		}
	}
}
void createCanalMap(vector<Canal> &map) {
	int numCages = 0;
	int x = 0;
	int y = 0;
	cin >> numCages;
	map.reserve(static_cast<size_t>(numCages));
	while(cin >> x >> y) {
		map.push_back(Canal(x,y));
	}
}


int smallestDistHelper(vector<Cages> &map, int currCage) {
	double inf = numeric_limits<double>::infinity();
	for (size_t i = 0; i < map.size(); ++i) {
		if (!map[i].getVisited() && 
			(map[i].getDistV() < inf)) {
				inf = map[i].getDistV();
				currCage = static_cast<int>(i);
		}
	}
	return currCage;
}
double euclideanDistance(int x1, int y1, int x2, int y2) {
	double term1 = static_cast<double>((x2-x1));
	double term2 = static_cast<double>((y2-y1));
	return sqrt((term1 * term1) + (term2 * term2));
}
void testDistHelper(vector<Cages> &map, size_t currCage) {

	double dist = 0;
	for (size_t i = 0; i < map.size(); ++i) {
		if (i == currCage) {
			//do nothing since no distance check is needed
		}
		else if (!map[i].getVisited()){

			dist = euclideanDistance(map[currCage].getXCoord()
							, map[currCage].getYCoord(), map[i].getXCoord()
							, map[i].getYCoord());
			switch(map[currCage].getDescription()) {
				case 'z':
					if ((map[i].getDescription() == 'z') ||
						(map[i].getDescription() == 'b')) {
						if (dist < map[i].getDistV()) {
							map[i].setDistV(dist);
							map[i].setPrevV(static_cast<int>(currCage));
						}
					}
					break;
				case 'w':
					if ((map[i].getDescription() == 'w') ||
						(map[i].getDescription() == 'b')) {
						if (dist < map[i].getDistV()) {
							map[i].setDistV(dist);
							map[i].setPrevV(static_cast<int>(currCage));
						}

					}
					break;
				case 'b':
					if (dist < map[i].getDistV()) {
						map[i].setDistV(dist);
						map[i].setPrevV(static_cast<int>(currCage));
					}
					break;
			}
		}
	}
}
void outputMST(vector<Cages> &map, ostringstream &os) {
	double sumEdges = 0;
	for (size_t i = 0; i < map.size(); ++i) {
		sumEdges += map[i].getDistV();
	}

	os << setprecision(2) << fixed << sumEdges << '\n';
	for (size_t j = map.size() - 1; j > 0; --j) {

		if (static_cast<int>(j) > map[j].getPrevV()) {
			os << map[j].getPrevV() << " " << j << '\n';
		}
		else {
			os << j << " " << map[j].getPrevV() << '\n';
		}
	}	
}
void implementMSTMode(vector<Cages> &map) {
	bool haveWild = false;
	bool haveBorder = false;
	bool haveZoo = false;

	for (size_t i = 0; i < map.size(); ++i) {
		if ((map[i].getXCoord() > 0) || ((map[i].getXCoord() <= 0) && 
			(map[i].getYCoord() > 0))) {
			haveZoo = true;
		}
		else if ((map[i].getXCoord() < 0) && (map[i].getYCoord() < 0)) {
			haveWild = true;
		} 
		else if (((map[i].getXCoord() <= 0) && (map[i].getYCoord() == 0)) || 
			((map[i].getXCoord() == 0) && (map[i].getYCoord() <= 0))){
			haveBorder = true;
		}
	}

	if (haveWild) {
		if (haveZoo && !haveBorder) {
			cerr << "Cannot construct MST" << '\n';
			exit(1);
		}
	}

	map[0].setPrevV(-1);
	map[0].setDistV(0);
	size_t numVertices = 1;
	int currentVertice = 0;

	while (numVertices < map.size()) {
		currentVertice = smallestDistHelper(map, currentVertice);
		map[static_cast<size_t>(currentVertice)].setVisited();
		++numVertices;
		testDistHelper(map, static_cast<size_t>(currentVertice));
	}
} 



void outputFTSP(vector<Canal> &map, vector<int> &path, ostringstream &os) {
	double sumEdges = 0;
	for(size_t i = 0; i < path.size(); ++i) {
		if ((i+1) < path.size()) {
			sumEdges += euclideanDistance(map[static_cast<size_t>(path[i])].getXCoord()
							, map[static_cast<size_t>(path[i])].getYCoord()
							, map[static_cast<size_t>(path[i+1])].getXCoord()
							, map[static_cast<size_t>(path[i+1])].getYCoord());
		}
		else {
			sumEdges += euclideanDistance(map[static_cast<size_t>(path[i])].getXCoord()
							, map[static_cast<size_t>(path[i])].getYCoord()
							, map[0].getXCoord(), map[0].getYCoord());
		}
	}

	os << setprecision(2) << fixed << sumEdges << '\n';

	for (size_t j = 0; j < path.size(); ++j) {
		os << path[j] << " ";
	}
}
void updateDistances(vector<Canal> &map, int newNode) {
	double dist = 0;
	for (size_t i = 0; i < map.size(); ++i) {
		if (!map[i].getVisited()) {
			dist = euclideanDistance(map[static_cast<size_t>(newNode)].getXCoord()
							, map[static_cast<size_t>(newNode)].getYCoord()
							, map[i].getXCoord(), map[i].getYCoord());
			if (dist < map[i].getDistV()) {
				map[i].setDistV(dist);
			}
		}
	}
}
int findFarthestCanal(vector<Canal> &map) {
	double currFarDist = 0;
	int farCanal = 0;
	for (size_t i = 0; i < map.size(); ++i) {
		if (!map[i].getVisited()) {
			if (map[i].getDistV() > currFarDist) {
				farCanal = static_cast<int>(i);
				currFarDist = map[i].getDistV();
			}
		}
	}
	return farCanal;
}
void insertionStep(vector<Canal> &map, vector<int> &route, int addNode) {
	double currMinDist = numeric_limits<double>::infinity();
	int innerPos = -1;

	for (size_t i = 0; i < route.size(); ++i) {
		if ((i + 1) < route.size()) {
			double result1 = euclideanDistance(map[static_cast<size_t>(route[i])].getXCoord()
								, map[static_cast<size_t>(route[i])].getYCoord()
								, map[static_cast<size_t>(addNode)].getXCoord()
								, map[static_cast<size_t>(addNode)].getYCoord()) ;
			double result2 = euclideanDistance(map[static_cast<size_t>(addNode)].getXCoord()
						, map[static_cast<size_t>(addNode)].getYCoord()
						, map[static_cast<size_t>(route[i+1])].getXCoord()
						, map[static_cast<size_t>(route[i+1])].getYCoord());

			double result3 = euclideanDistance(map[static_cast<size_t>(route[i])].getXCoord()
						, map[static_cast<size_t>(route[i])].getYCoord()
						, map[static_cast<size_t>(route[i+1])].getXCoord()
						, map[static_cast<size_t>(route[i+1])].getYCoord());

			if ((result1+result2-result3) < currMinDist) {
				currMinDist = result1+result2-result3;
				innerPos = static_cast<int>(i) + 1;
			}
		}
		else {

			double result1 = euclideanDistance(map[static_cast<size_t>(route[i])].getXCoord()
								, map[static_cast<size_t>(route[i])].getYCoord()
								, map[static_cast<size_t>(addNode)].getXCoord()
								, map[static_cast<size_t>(addNode)].getYCoord());
			double result2 = euclideanDistance(map[static_cast<size_t>(addNode)].getXCoord()
						, map[static_cast<size_t>(addNode)].getYCoord()
						, map[0].getXCoord()
						, map[0].getYCoord());
			double result3 = euclideanDistance(map[static_cast<size_t>(route[i])].getXCoord()
						, map[static_cast<size_t>(route[i])].getYCoord()
						, map[0].getXCoord(), map[0].getYCoord());
			if ((result1+result2-result3) < currMinDist) {
				innerPos = static_cast<int>(route.size());
			}
		}
	}

	if (static_cast<size_t>(innerPos) == route.size()) {
		map[static_cast<size_t>(addNode)].setVisited();
		route.push_back(addNode);
	}
	else {
		map[static_cast<size_t>(addNode)].setVisited();
		route.insert(route.begin() + innerPos, addNode);
	}
}
void implementFASTTSPMode(vector<Canal> &map, ostringstream &out) {	
	vector<int> path;
	path.reserve(map.size());
	int currFarthest = 0;

	path.push_back(0);
	map[0].setVisited();

	map[static_cast<size_t>(currFarthest)].setVisited();
	while(path.size() < map.size()) {
		updateDistances(map, currFarthest);
		currFarthest = findFarthestCanal(map);
		insertionStep(map, path, currFarthest);
	}

	outputFTSP(map, path, out);
	cout << out.str();	
}



vector<vector<double>> prepWork(vector<Canal> &map) {
	createCanalMap(map);
	vector<vector<double>> distanceMatrix(map.size(), vector<double>(map.size(), 0));
	for (size_t i = 0; i < map.size(); ++i) {
		for (size_t j = 0; j <map.size(); ++j) {
			if (i == j) {
				// do nothing
			}
			else {
				distanceMatrix[i][j] = euclideanDistance(map[i].getXCoord(),
										map[i].getYCoord(), map[j].getXCoord(),
										map[j].getYCoord());
			}
		}
	}

	return distanceMatrix;
}
double findUpperBound(vector<Canal> &map) {
	vector<int> path;
	path.reserve(map.size());
	int currFarthest = 0;

	path.push_back(0);
	map[0].setVisited();

	map[static_cast<size_t>(currFarthest)].setVisited();
	while(path.size() < map.size()) {
		updateDistances(map, currFarthest);
		currFarthest = findFarthestCanal(map);
		insertionStep(map, path, currFarthest);
	}

	double sumEdges = 0;
	for(size_t i = 0; i < path.size(); ++i) {
		if ((i+1) < path.size()) {
			sumEdges += euclideanDistance(map[static_cast<size_t>(path[i])].getXCoord()
							, map[static_cast<size_t>(path[i])].getYCoord()
							, map[static_cast<size_t>(path[i+1])].getXCoord()
							, map[static_cast<size_t>(path[i+1])].getYCoord());
		}
		else {
			sumEdges += euclideanDistance(map[static_cast<size_t>(path[i])].getXCoord()
							, map[static_cast<size_t>(path[i])].getYCoord()
							, map[0].getXCoord(), map[0].getYCoord());
		}
	}

	optimalPath = path;
	return sumEdges;
}
double estimateMST(deque<int> &unused, vector<vector<double>> &distanceMatrix) {

	double lowBound = 0;
	vector<bool> visited(unused.size());
	vector<double> distances(unused.size());
	fill(visited.begin(), visited.end(), false);
	fill(distances.begin(), distances.end(), numeric_limits<double>::infinity());

	distances[0] = 0;
	size_t numVertices = 0;
	size_t currentVertice = 0;

	while (numVertices < unused.size()) {
		double inf = numeric_limits<double>::infinity();
		for (size_t i = 0; i < unused.size(); ++i) {
			if (!visited[i] && (distances[i] < inf)) {
				inf = distances[i];
				currentVertice = i;
			}
		}//End for
		visited[static_cast<size_t>(currentVertice)] = true;
		++numVertices;
		lowBound += inf;

		for (size_t j = 0; j < unused.size(); ++j) {
			if (!visited[j]) {
				double tempDist = distanceMatrix[static_cast<size_t>(unused[currentVertice])]
											[static_cast<size_t>(unused[j])];
				if (distances[j] > tempDist) {
					distances[j] = tempDist;
				}
			}//end if
		}//end for
	}//end while

	return lowBound;
}//end EstimateMST
bool promising(deque<int> &unused, vector<int> &path, double currBestDist
				, vector<vector<double>> &distMat) {

	if (unused.size() < 4) {
		return true;
	}
	double mstDist = estimateMST(unused, distMat);
// find edges to connect
	double startMinEdge = numeric_limits<double>::infinity();
	double endMinEdge = numeric_limits<double>::infinity();
	for (size_t j = 0; j < unused.size(); ++j) {
		if (distMat[static_cast<size_t>(path[0])]
				[static_cast<size_t>(unused[j])] < startMinEdge) {

			startMinEdge = distMat[static_cast<size_t>(path[0])]
									[static_cast<size_t>(unused[j])];
		}
		if (distMat[static_cast<size_t>(path.back())]
					[static_cast<size_t>(unused[j])] < endMinEdge) {
			endMinEdge = distMat[static_cast<size_t>(path.back())]
								[static_cast<size_t>(unused[j])];
		}
	}
	return ((currPathDistance + mstDist + startMinEdge + endMinEdge) < currBestDist);
}


void genPerms(deque<int> &unused, vector<int> &path, double &currBestDist
				, vector<vector<double>> &distanceMatrix) {
	if (unused.empty()) {
		currPathDistance += distanceMatrix[static_cast<size_t>(path.back())]
										[static_cast<size_t>(path.front())];

		if (currPathDistance < currBestDist) {
			currBestDist = currPathDistance;
			optimalPath = path;
		}//end if

		currPathDistance -= distanceMatrix[static_cast<size_t>(path.back())]
										[static_cast<size_t>(path.front())];
		return;
	}//end if
	if (!promising(unused, path, currBestDist, distanceMatrix)) {
		return;
	}
	for (size_t i = 0; i != unused.size(); ++i) {
		double someDist = distanceMatrix[static_cast<size_t>(path.back())]
										[static_cast<size_t>(unused.front())];
		currPathDistance += someDist;
		path.push_back(unused.front());
		unused.pop_front();
		genPerms(unused, path, currBestDist, distanceMatrix);
		unused.push_back(path.back());
		path.pop_back();
		currPathDistance -= someDist;
	}
}

void implementOPTTSPMode(vector<Canal> &map, double bestPathDist
						, vector<vector<double>> &distanceMatrix
						, ostringstream &os) {

	deque<int> unused;
	vector<int> path;
	path.reserve(map.size());

	path.push_back(0);
	for (size_t i = 1; i < map.size(); ++i) {
		unused.push_back(static_cast<int>(i));
	}

	genPerms(unused, path, bestPathDist, distanceMatrix);
	os << setprecision(2) << fixed << bestPathDist << '\n';

	for (size_t j = 0; j < optimalPath.size(); ++j) {

		os << optimalPath[j] << " ";
	}

	os << '\n';
	cout << os.str();
}


int main(int argc, char *argv[]) {
	ios_base::sync_with_stdio(false);


	ostringstream out;
	char zooMode = getMode(argc, argv);

	switch (zooMode) {
		case 'M' :
		{
			vector<Cages> map;
			createZooMap(map);
			implementMSTMode(map);
			outputMST(map, out);
			cout << out.str();
			break;
		}
		case 'F' :
		{
			vector<Canal> rivers;
			createCanalMap(rivers);
			implementFASTTSPMode(rivers, out);
			break;
		}
		case 'O' :
			vector<Canal> optPath;
			vector<vector<double>> distanceMatrix = prepWork(optPath);
			double currBestDist = findUpperBound(optPath);
			implementOPTTSPMode(optPath, currBestDist, distanceMatrix, out);
			break;
	}

}