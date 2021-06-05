#include <barrier>
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

using namespace std;

int nw;
int occurancesX;
int s;
int X;
int num_nodes = 100;
int min_edges_per_node = 1;
int max_edges_per_node = 5;

// 4609352
// 4461896
// 377439
template <class T> void swap (T& a, T& b)
{
  T c(std::move(a)); a=std::move(b); b=std::move(c);
}
int main(int argc, char** argv) {
    

    s = atoi(argv[1]); // We get the source nodeID
    X = atoi(argv[2]); // We get the value to compute the occurancesX of
    nw = atoi(argv[3]); // We get the value to compute the occurancesX of
    
    if(argc == 7) {
        num_nodes = atoi(argv[4]); // We get the number of nodes to be generated 
        min_edges_per_node = atoi(argv[5]); // We get the minimum number of edges per node
        max_edges_per_node = atoi(argv[6]); // We get the maximum number of edges per node
    }
    
    auto filename = "data/" + to_string(num_nodes) + "_" + to_string(min_edges_per_node) + "_" + to_string(max_edges_per_node) + ".txt";
    printf ("Running par with: %d threads, %d nodes which have a minimum of %d edges and a maximum of %d edges\n", nw, num_nodes, min_edges_per_node, max_edges_per_node);
    
    mutex currentQueueGuarder;
    mutex checkCompletionGuarder;
    mutex tryStealGuarder;
    mutex checkProcessLock;
    mutex m;
    vector<thread> threads;
    Graph<int> graph;

    {   utimer tpg("process graph");
        processGraph(&graph, filename);
    }
    Node<int> source = graph.getNode(s);
    vector<Edge> init_edges = source.getOutboundEdges();
    vector<bool> processed(num_nodes);
    vector< queue< Node<int> > >  firstQueues(nw);
    vector< queue< Node<int> > >  secondQueues(nw);
    barrier work_done{nw};
    
    // We run condition 
    processed[source.getNodeID()] = true;
    if(source.getVal() == X) occurancesX++;
    for(int i = 0; i < init_edges.size(); i++) {
        if (!processed[init_edges[i].getDestID()]) {
            firstQueues[nw > 1 ? i % nw : 0].push(graph.getNode(init_edges[i].getDestID()));
            processed[init_edges[i].getDestID()] = true;
        }
    }
    auto bfs = [&](int tid) {
        queue< Node<int> > currentQueue = firstQueues[tid];
        queue< Node<int> > nextQueue = secondQueues[tid];
        bool done = false;
        int localX = 0;
        bool firstQueue = true;        
        while (!done) {
            while (!firstQueues[tid].empty()) {
                // currentQueueGuarder.lock();
                Node<int> current = firstQueues[tid].front();
                firstQueues[tid].pop();
                // currentQueueGuarder.unlock();
                if (current.getVal() == X) localX ++;
                vector<Edge> outboundEdges = current.getOutboundEdges();
                
                for (int i = 0; i < outboundEdges.size(); ++i) {
                    if (!processed[outboundEdges[i].getDestID()]) {
                        // checkProcessLock.lock();
                        processed[outboundEdges[i].getDestID()] = true;
                        // checkProcessLock.unlock();
                        secondQueues[tid].push(graph.getNode(outboundEdges[i].getDestID()));
                    }
                }
                
            }
            done = true;
            // tryStealGuarder.lock();
            for (int i = 0; i < nw; ++i) {
                done |= !firstQueues[i].empty();
            }
            // tryStealGuarder.unlock();

            if (!done) {
                for (int i = 0; i < nw; ++i) {
                    if (firstQueues[i].size() > 1) {
                        // currentQueueGuarder.lock();
                        firstQueues[tid].push(firstQueues[i].front());
                        firstQueues[i].pop();
                        // currentQueueGuarder.unlock();
                        break;
                    }
                }
            } else {
                //printf("I am thread %d and i am beforeeee \n", tid);
                work_done.arrive_and_wait(); // Barrier to wait next iteration
                if(tid == 0) swap(firstQueues, secondQueues);
                work_done.arrive_and_wait(); 
                //printf("I am thread %d and i am after \n", tid);
                currentQueue = firstQueues[tid];
                nextQueue = secondQueues[tid]; 
                if(!firstQueues[tid].empty()) done = false; // Continue if there's still work
            }
        }
        checkCompletionGuarder.lock();
        occurancesX += localX;
        checkCompletionGuarder.unlock();
    };
    {   utimer tpg("par time ");
        for(int i=0;i<nw;i++) {
            threads.emplace_back(bfs, i);
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }
     
    printf ("The number of instances of: %d is %d \n",  X, occurancesX);
    return 0;
}
