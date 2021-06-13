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
    
    auto filename = "data/graphs/" + to_string(num_nodes) + "_" + to_string(min_edges_per_node) + "_" + to_string(max_edges_per_node) + ".txt";
    printf ("SEQCPP:%d:%d:%d\n",  num_nodes, min_edges_per_node, max_edges_per_node);
    Graph<int> g;
    {   utimer tpg("Graph");
        processGraph(&g, filename);
    }

    {   utimer tsbfs("SERIAL_TIME");
        vector<bool> visited(num_nodes, false);
        queue< Node<int> > q;
        Node<int> current;
        q.push(g.getNode(s));
        visited[s] = true;

        while (!q.empty())
        {
#ifdef WITHTIME 
            {   utimer tpg("Time 1 while loop");
#endif
            current = q.front();
            q.pop();

#ifdef WAIT
            usleep(5000);
#endif
#ifdef WITHTIME 
            {   utimer tpg("Time to check X update");
#endif
            if (current.getVal() == X) {
                occurancesX++;
            }
#ifdef WITHTIME 
            }
#endif
            vector<Edge> outboundEdges = current.getOutboundEdges();
#ifdef WITHTIME 
            {   utimer tpg("Time for edges");
#endif
            for (long unsigned int i = 0; i < outboundEdges.size(); ++i) {
                if (!visited[outboundEdges[i].getDestID()]) {
                    q.push(g.getNode(outboundEdges[i].getDestID()));
                    visited[outboundEdges[i].getDestID()] = true;
                }
            }
#ifdef WITHTIME 
            }
#endif
            
        }
    }
    printf ("X = %d has %d instances \n",  X, occurancesX);
    return 0;
}