#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>
#include <semaphore.h>
#include <numeric>
#include "utils.cpp"
#include "utimer.cpp"
#include <unistd.h>
#include "h/barrier.hpp"
#include "src/bfs.cpp"

using namespace std;

int nw;
int occurancesX;
int s;
int X;
int num_nodes = 100;
int min_edges_per_node = 1;
int max_edges_per_node = 5;




int main(int argc, char** argv) {
    

    s = atoi(argv[1]); // We get the source nodeID
    X = atoi(argv[2]); // We get the value to compute the occurancesX of
    nw = atoi(argv[3]); // We get the value to compute the occurancesX of
    if(argc == 7) {
        num_nodes = atoi(argv[4]); // We get the number of nodes to be generated 
        min_edges_per_node = atoi(argv[5]); // We get the minimum number of edges per node
        max_edges_per_node = atoi(argv[6]); // We get the maximum number of edges per node
    }
    
    auto filename = "data/graphs/" + to_string(num_nodes) + "_" + to_string(min_edges_per_node) + "_" + to_string(max_edges_per_node) + ".txt";
    printf ("PARCPP:%d:%d:%d:%d\n", nw, num_nodes, min_edges_per_node, max_edges_per_node);
    
    vector<thread> threads;
    Graph<int> graph;

    {   utimer tpg("process graph");
        processGraph(&graph, filename);
    }
    Node<int> source = graph.getNode(s);
    vector<bool> processed(num_nodes);
    vector< queue< Node<int> > >  firstQueues(nw);
    vector< queue< Node<int> > >  secondQueues(nw);
    MultilevelBarrier work_done{nw};
    
    // We run condition 
    {   utimer tpg("Dispatch");
        processed[source.getNodeID()] = true;
        if(source.getVal() == X) occurancesX++;       
#ifdef WAIT
            usleep(5000); // We assume we make work on the current element being processed
#endif        
        vector<Edge> init_edges = source.getOutboundEdges();
        for(long unsigned int i = 0; i < init_edges.size(); i++) {
            if (!processed[init_edges[i].getDestID()]) {
                firstQueues[nw > 1 ? i % nw : 0].push(graph.getNode(init_edges[i].getDestID()));
                processed[init_edges[i].getDestID()] = true;
            }
        }
    }
    {   utimer tpg("PAR_TIME");
        for(int i=0;i<nw;i++) {
            threads.emplace_back(
                bfs, 
                i, 
                nw,
                std::ref(X), 
                std::ref(occurancesX), 
                std::ref(graph), 
                std::ref(firstQueues), 
                std::ref(secondQueues),
                std::ref(processed),
                std::ref(work_done)
            );
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }
     
    printf ("X = %d has %d instances \n",  X, occurancesX);
    return 0;
}
