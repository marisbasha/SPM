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
    // Worker( vector<bool>* processed,
    //         vector< queue< Node<int> > >*  firstQueues,
    //         vector< queue< Node<int> > >*  secondQueues):
    //         processed(processed), firstQueues(firstQueues), secondQueues(secondQueues) {}
    // vector<bool>* processed;
    // vector< queue< Node<int> > >*  firstQueues;
    // vector< queue< Node<int> > >*  secondQueues;
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
        int localX = 0;
        bool firstQueue = true;        
        while (!done) {
            while (!firstQueues[tid].empty()) {
                currentQueueGuarder.lock();
                Node<int> current = firstQueues[tid].front();
                firstQueues[tid].pop();
                currentQueueGuarder.unlock();
                if (current.getVal() == X) localX ++;
                usleep(1000); // We assume we make work on the current element being processed
                vector<Edge> outboundEdges = current.getOutboundEdges();
                for (int i = 0; i < outboundEdges.size(); ++i) {
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
        checkCompletionGuarder.lock();
        occurancesX += localX;
        checkCompletionGuarder.unlock();
        return 0;
    }    
};

int main(int argc, char *argv[]) {
    
    s = atoi(argv[1]); // We get the source nodeID
    X = atoi(argv[2]); // We get the value to compute the occurancesX of
    nw = atoi(argv[3]); // We get the value to compute the occurancesX of
    
    if(argc == 7) {
        num_nodes = atoi(argv[4]); // We get the number of nodes to be generated 
        min_edges_per_node = atoi(argv[5]); // We get the minimum number of edges per node
        max_edges_per_node = atoi(argv[6]); // We get the maximum number of edges per node
    }
    
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
        for(int i = 0; i < init_edges.size(); i++) {
            if (!processed[init_edges[i].getDestID()]) {
                firstQueues[nw > 1 ? i % nw : 0].push(graph.getNode(init_edges[i].getDestID()));
                processed[init_edges[i].getDestID()] = true;
            }
        }
    }

    {   utimer tpg("process ff par");
        // ff_farm farm;
        // std::vector<ff_node*> w;
        // for(int i=0;i<nw;++i)
        //     w.push_back(new Worker(&p));
        // farm.add_workers(w);
        // farm.add_emitter(new Emitter(w, &p));
        // farm.wrap_around();
        // if (farm.run_then_freeze()<0) {
        //     error("running farm ff_graphsearch\n");
        // } else farm.wait_freezing();
        Worker** N = new Worker*[nw];
        bar.barrierSetup(nw);
        int iter = 0;
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
    printf ("The number of instances of:: %d is %d \n",  X, occurancesX);
    return 0;
}

