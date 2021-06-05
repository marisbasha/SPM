#include <ctime>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <string> 
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <bits/stdc++.h>

using namespace std;

int main(int argc, char** argv)
{
    int num_nodes = atoi(argv[1]); // We get the number of nodes to be generated 
    int min_edges_per_node = atoi(argv[2]); // We get the minimum number of edges per node
    int max_edges_per_node = atoi(argv[3]); // We get the maximum number of edges per node
    auto filename = "data/" +to_string(num_nodes) + "_" + to_string(min_edges_per_node) + "_" + to_string(max_edges_per_node) + ".txt";

    int max_node_value = 121; // The maximum value of the node
    int num_edges; // The number of edges that will be calculated randomly foreach node
    int randID;
    vector<int> indices; // Set of indices 
    vector<int> nodeIDs(num_nodes, 0);
    vector<pair<int, int>> edgesCombinationsInserted;
    pair<int, int>p = make_pair(0, 0);
    ofstream graph(filename);
    auto pairFound = false;
    graph << num_nodes << endl; // We write the number of nodes so we know when to stop when we parse it
    for (int i = 0; i < num_nodes; ++i) {
        nodeIDs.push_back(i);
        graph << (i) << " " << (rand() % max_node_value + 1) << '\n'; // We insert in each line the nodeID and a random value from 1 to 120 that will be the node value 
    }

    for (int i = 0; i < num_nodes; ++i) {
        num_edges = rand() % max_edges_per_node + min_edges_per_node; // We define a number of nodes
        indices = {i};
        while(num_edges > 0) {
            randID = nodeIDs[rand() % nodeIDs.size()]; 
            if(find(indices.begin(), indices.end(), randID) == indices.end()) {
                indices.push_back(randID);
                graph << (i) << " " << (randID) << '\n'; // We insert a line foreach edge the current node has while we have num_edges available to insert avoiding duplicates or the insertion of i
                num_edges--;
            }
        }
        cout << "Finished node with ID: " << i << endl;
    }

    graph.flush();
    return 0;
}