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
#include <unistd.h>
#include <mutex>

#include <ff/ff.hpp>
#include <ff/node.hpp>

#include "utils.cpp"
#include "utimer.cpp"


using namespace ff;
using namespace std;


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
mutex decreaseActive;
mutex checkProcessLock;
Graph<int> graph;
vector<bool> processed;
int activeThreads;
vector< queue< Node<int> > >  firstQueues;
vector< queue< Node<int> > >  secondQueues;
int goforanother = 0;
int goforanother2 = 0;



class Barrier_
{
public:
  Barrier_() {};
  Barrier_(const Barrier_&) = delete;
  Barrier_& operator=(const Barrier_&) = delete;
  Barrier_( int count) :
    passed_barrier(0), iteration_number(0), 
    passed_barrier_reset_value(count)
  {}
  void setCount(int count) {
    passed_barrier_reset_value = count;
    passed_barrier= 0;
    iteration_number=0;
  }
  void decrease_active_threads(){
    passed_barrier_reset_value = (passed_barrier_reset_value == 0 ? 0 : passed_barrier_reset_value - 1);
  }
  void count_down_and_wait()
  {
    int gen = iteration_number.load();
    if ( ++passed_barrier >= passed_barrier_reset_value) {
      if (iteration_number.compare_exchange_weak(gen, gen + 1)) {
        passed_barrier = 0;
      }
      return;
    }

    while ((gen == iteration_number) && (passed_barrier <= passed_barrier_reset_value))
      std::this_thread::yield();
  }

private:
  std::atomic< int> passed_barrier;
  std::atomic< int> iteration_number;
  int passed_barrier_reset_value;
};

Barrier_ work_done;

class Worker:public ff_node {
public:
    Worker(int t=-1): t(t) {}
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
        int localX = 0;
        while (!done) {
            while (!currentQueue.empty()) {
                currentQueueGuarder.lock();
                Node<int> current = currentQueue.front();
                currentQueue.pop();
                currentQueueGuarder.unlock();
                if (current.getVal() == X) localX ++;
                //usleep(5000); // We assume we make work on the current element being processed
                vector<Edge> outboundEdges = current.getOutboundEdges();
                for (long unsigned int i = 0; i < outboundEdges.size(); ++i) {
                    if (!processed[outboundEdges[i].getDestID()]) {
                        checkProcessLock.lock();
                        processed[outboundEdges[i].getDestID()] = true;
                        checkProcessLock.unlock();
                        nextQueue.push(graph.getNode(outboundEdges[i].getDestID()));
                    }
                }
                
            }
            done = true;
            
            for (int i = 0; i < nw; ++i) {
                done |= !firstQueues[i].empty();
            }
            currentQueueGuarder.lock();
            if (!done) {
                for (int i = 0; i < nw; ++i) {
                    if (firstQueues[i].size() > 1) {
                        currentQueue.push(firstQueues[i].front());
                        firstQueues[i].pop();
                        break;
                    }
                }
            }
            currentQueueGuarder.unlock();
            if(done) {
                work_done.count_down_and_wait(); // Barrier to wait next iteration
                queue<Node<int>> t = currentQueue;
                currentQueue = nextQueue;
                nextQueue = t;
                work_done.count_down_and_wait(); // Barrier to wait next iteration
                if(!currentQueue.empty()) { done = false; }
            }
            if(done) {
                decreaseActive.lock();
                work_done.decrease_active_threads();
                decreaseActive.unlock();
            }
        }
        checkCompletionGuarder.lock();
        occurancesX += localX;
        checkCompletionGuarder.unlock();
        
        return 0;
    }  

protected:
    int t;    
};

int main(int argc, char *argv[]) {
    
    s = atoi(argv[1]); // We get the source nodeID
    X = atoi(argv[2]); // We get the value to compute the occurancesX of
    nw = atoi(argv[3]); // We get the value to compute the occurancesX of
    activeThreads = nw;
    if(argc > 6) {
        num_nodes = atoi(argv[4]); // We get the number of nodes to be generated 
        min_edges_per_node = atoi(argv[5]); // We get the minimum number of edges per node
        max_edges_per_node = atoi(argv[6]); // We get the maximum number of edges per node
    }

    if(argc > 7 && !strcmp(argv[7], "farm")) is_farm = true;

    
    auto filename = "data/" + to_string(num_nodes) + "_" + to_string(min_edges_per_node) + "_" + to_string(max_edges_per_node) + ".txt";
    printf ("GRAPHCPP:%d:%d:%d:%d:%d\n", nw, num_nodes, min_edges_per_node, max_edges_per_node, is_farm);
    {   utimer tpg("Graph");
        processGraph(&graph, filename);
    }
    Node<int> source = graph.getNode(s);
    processed.resize(num_nodes);
    firstQueues.resize(nw);
    secondQueues.resize(nw);

    // We run condition 
    {   utimer tpg("Dispatch");
        processed[source.getNodeID()] = true;
        if(source.getVal() == X) occurancesX++;
        //usleep(5000); // We assume we make work on the current element being processed
        vector<Edge> init_edges = source.getOutboundEdges();
        for(long unsigned int i = 0; i < init_edges.size(); i++) {
            if (!processed[init_edges[i].getDestID()]) {
                firstQueues[nw > 1 ? i % nw : 0].push(graph.getNode(init_edges[i].getDestID()));
                processed[init_edges[i].getDestID()] = true;
            }
        }
    }
    

    {   utimer tpg("FF_TIME");
        work_done.setCount(nw);
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
                N[i]= new Worker(i);
            }
            for(int i=0;i<nw;++i) {
                N[i]->run();
            }
            for(int i=0;i<nw;++i) {
                N[i]->wait();
            } 
        }  
    }
    printf ("X = %d has %d instances \n",  X, occurancesX);
    return 0;
}

