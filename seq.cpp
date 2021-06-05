#include <queue>
#include <fstream>
#include <sstream>
#include <iostream>
#include <semaphore.h>
#include "utils.cpp"
#include "utimer.cpp"
#include <unistd.h>

using namespace std;


int occurancesX = 0;
int s;
int X;
int num_nodes = 100;
int min_edges_per_node = 1;
int max_edges_per_node = 5;


int main(int argc, char** argv) {
    s = atoi(argv[1]); // We get the source nodeID
    X = atoi(argv[2]); // We get the value to compute the occurancesX of
    if(argc == 6) {
        num_nodes = atoi(argv[3]); // We get the number of nodes to be generated 
        min_edges_per_node = atoi(argv[4]); // We get the minimum number of edges per node
        max_edges_per_node = atoi(argv[5]); // We get the maximum number of edges per node
    }
    
    auto filename = "data/" + to_string(num_nodes) + "_" + to_string(min_edges_per_node) + "_" + to_string(max_edges_per_node) + ".txt";
    printf ("Running seq with: %d nodes which have a minimum of %d edges and a maximum of %d edges\n",  num_nodes, min_edges_per_node, max_edges_per_node);
    cout << filename << endl;
    Graph<int> g;
    {   utimer tpg("process graph");
        processGraph(&g, filename);
    }

    auto bfs = [&](){
        vector<bool> visited(num_nodes, false);
        queue< Node<int> > q;
        q.push(g.getNode(s));
        visited[s] = true;
        while (!q.empty())
        {
            Node<int> current = q.front();
            q.pop();
            if (current.getVal() == X) {
                occurancesX++;
            }
            vector<Edge> outboundEdges = current.getOutboundEdges();
            for (int i = 0; i < outboundEdges.size(); ++i) {
                if (!visited[outboundEdges[i].getDestID()]) {
                    q.push(g.getNode(outboundEdges[i].getDestID()));
                    visited[outboundEdges[i].getDestID()] = true;
                }
            }
            
        }
    };
    {   utimer tsbfs("serial bfs");
        bfs();
    }
    printf ("The number of instances of:: %d is %d \n",  X, occurancesX);
    return 0;
}