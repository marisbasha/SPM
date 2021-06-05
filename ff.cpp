#if !defined(FF_INITIAL_BARRIER)
// to run this test we need to be sure that the initial barrier is executed
#define FF_INITIAL_BARRIER
#endif

#include <vector>
#include <cstdio>
#include <queue>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <semaphore.h>
#include <numeric>
#include <ff/ff.hpp>
#include <ff/node.hpp>
#include <ff/barrier.hpp>
#include "utils.cpp"
#include "utimer.cpp"
#include <unistd.h>

using namespace ff;
using namespace std;
static barrierSelector<0> bar;
#define BARRIER   bar.doBarrier(get_my_id())


int nw;
int occurancesX;
int s;
int X;
int num_nodes = 100;
int min_edges_per_node = 1;
int max_edges_per_node = 5;
bool is_farm;
mutex currentQueueGuarder;
mutex checkCompletionGuarder;
mutex tryStealGuarder;
mutex checkProcessLock;
Graph<int> graph;
vector<bool> processed;
vector< queue< Node<int> > >  firstQueues;
vector< queue< Node<int> > >  secondQueues;

class Worker:public ff_node {
public:
    Worker(int t = -1): t(t) {}
    int svc_init() {
        if(t != -1) set_id(t);
        return 0;
    }
    int run(bool=false)  { return ff_node::run();}
    int wait()           { return ff_node::wait();}
    void set_id(ssize_t id) {
        ff_node::set_id(id);
    }
    void *svc(void*) {
        int tid = get_my_id();
	    queue< Node<int> > currentQueue = firstQueues[tid];
        queue< Node<int> > nextQueue = secondQueues[tid];
        bool done = false;
        while (!done) {
            while (!firstQueues[tid].empty()) {
                currentQueueGuarder.lock();
                Node<int> current = firstQueues[tid].front();
                firstQueues[tid].pop();
                currentQueueGuarder.unlock();
                if (current.getVal() == X) localX ++;
                usleep(1000); // We assume we make work on the current element being processed
                vector<Edge> outboundEdges = current.getOutboundEdges();
                for (long unsigned int i = 0; i < outboundEdges.size(); ++i) {
                    if (!processed[outboundEdges[i].getDestID()]) {
                        checkProcessLock.lock();
                        processed[outboundEdges[i].getDestID()] = true;
                        checkProcessLock.unlock();
                        secondQueues[tid].push(graph.getNode(outboundEdges[i].getDestID()));
                    }
                }
                
            }
            done = true;
            for (int i = 0; i < nw; ++i) {
                done |= !firstQueues[i].empty();
            }
            if (!done) {
                currentQueueGuarder.lock();
                for (int i = 0; i < nw; ++i) {
                    if (firstQueues[i].size() > 1) {
                        firstQueues[tid].push(firstQueues[i].front());
                        firstQueues[i].pop();
                        break;
                    }
                }
                currentQueueGuarder.unlock();
            } else {
                //printf("I am thread %d and i am beforeeee \n", tid);
                BARRIER; // Barrier to wait next iteration
                if(tid == 0) swap(firstQueues, secondQueues);
                BARRIER;
                currentQueue = firstQueues[tid];
                nextQueue = secondQueues[tid]; 
                if(!currentQueue.empty()) done = false; // Continue if there's still work
            }
        }
        
        return 0;
    }  
    void  svc_end()  {
        checkCompletionGuarder.lock();
        occurancesX += localX;
        checkCompletionGuarder.unlock();
    }  
protected:
    int t;
    int localX;
};

int main(int argc, char *argv[]) {
    
    s = atoi(argv[1]); // We get the source nodeID
    X = atoi(argv[2]); // We get the value to compute the occurancesX of
    nw = atoi(argv[3]); // We get the value to compute the occurancesX of
    
    if(argc > 6) {
        num_nodes = atoi(argv[4]); // We get the number of nodes to be generated 
        min_edges_per_node = atoi(argv[5]); // We get the minimum number of edges per node
        max_edges_per_node = atoi(argv[6]); // We get the maximum number of edges per node
    }

    if(argc > 7 && !strcmp(argv[7], "farm")) is_farm = true;

    
    auto filename = "data/" + to_string(num_nodes) + "_" + to_string(min_edges_per_node) + "_" + to_string(max_edges_per_node) + ".txt";
    printf ("Running ff with: %d threads, %d nodes which have a minimum of %d edges and a maximum of %d edges\n", nw, num_nodes, min_edges_per_node, max_edges_per_node);
    
    

    {   utimer tpg("process graph");
        processGraph(&graph, filename);
    }
    Node<int> source = graph.getNode(s);
    processed.resize(num_nodes);
    firstQueues.resize(nw);
    secondQueues.resize(nw);

    // We run condition 
    {   utimer tpg("Dispatch jobs of source node to different queues");
        processed[source.getNodeID()] = true;
        if(source.getVal() == X) occurancesX++;
        usleep(1000); // We assume we make work on the current element being processed
        vector<Edge> init_edges = source.getOutboundEdges();
        for(long unsigned int i = 0; i < init_edges.size(); i++) {
            if (!processed[init_edges[i].getDestID()]) {
                firstQueues[nw > 1 ? i % nw : 0].push(graph.getNode(init_edges[i].getDestID()));
                processed[init_edges[i].getDestID()] = true;
            }
        }
    }

    {   utimer tpg("process ff par");
        bar.barrierSetup(nw);
        if(is_farm) {
            ff_farm farm;
            vector<ff_node*> w;
            for(int i=0;i<nw;++i)
                w.push_back(new Worker(i));
            farm.add_workers(w);
            if (farm.run_then_freeze()<0) {
                error("Running bfs par ff \n");
            } else farm.wait_freezing();
        } 
        else {
            Worker** N = new Worker*[nw];
            for(int i=0;i<nw;++i) {
                N[i]= new Worker();
                N[i]->set_id(i);
            }
            for(int i=0;i<nw;++i) {
                N[i]->run();
            }
            for(int i=0;i<nw;++i) {
                N[i]->wait();
            } 
        }  
    }
    printf ("The number of instances of X = %d is %d \n",  X, occurancesX);
    return 0;
}

