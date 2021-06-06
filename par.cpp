// #include <Barrier>
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
#include <mutex>
#include <atomic>
using namespace std;

int nw;
int occurancesX;
int s;
int X;
int num_nodes = 100;
int min_edges_per_node = 1;
int max_edges_per_node = 5;
mutex currentQueueGuarder;
mutex checkCompletionGuarder;
mutex decreaseActive;
mutex checkProcessLock;


class Barrier
{
public:
  Barrier(const Barrier&) = delete;
  Barrier& operator=(const Barrier&) = delete;
  explicit Barrier( int count) :
    passed_barrier(0), iteration_number(0), 
    passed_barrier_reset_value(count)
  {
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
    Barrier work_done{nw};
    
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
    auto bfs = [&](int tid) {
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
            if (!done) {
                
                for (int i = 0; i < nw; ++i) {
                    if (firstQueues[i].size() > 1) {
                        currentQueueGuarder.lock();
                        currentQueue.push(firstQueues[i].front());
                        firstQueues[i].pop();
                        currentQueueGuarder.unlock();
                        break;
                    }
                }
                
            } else {
                //printf("I am thread %d and i am beforeeee with firstQueues[tid].: %d and secondQueues[tid]: %d  \n", tid, currentQueue.size(), nextQueue.size());
                work_done.count_down_and_wait(); // Barrier to wait next iteration
                queue<Node<int>> t = currentQueue;
                currentQueue = nextQueue;
                nextQueue = t;
                work_done.count_down_and_wait(); // Barrier to wait next iteration
                //printf("After wait I am thread %d and i am beforeeee with firstQueues[tid].: %d and secondQueues[tid]: %d  \n", tid, currentQueue.size(), nextQueue.size());
                if(!currentQueue.empty()) { done = false; } // Continue if there's still work
                
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
    };
    {   utimer tpg("PAR_TIME");
        for(int i=0;i<nw;i++) {
            threads.emplace_back(bfs, i);
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }
     
    printf ("X = %d has %d instances \n",  X, occurancesX);
    return 0;
}
