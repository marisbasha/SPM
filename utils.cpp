#include <iostream>
#include <fstream>
#include <string>
#include "h/graph.h"

using namespace std;

void processGraph(Graph<int>* graph, string filename) {
	ifstream file(filename);
    int nodeCount;
	file >> nodeCount;
	
	for (int i = 0; i < nodeCount; ++i) {
		int nodeID, val;
		file >> nodeID >> val;
		graph->addNode(nodeID, val);
	}
    while( file.peek() != EOF ) {
        int src, dest;
		file >> src >> dest;
		graph->addEdge(src, dest);
        // graph->addEdge(dest, src);
    }
}