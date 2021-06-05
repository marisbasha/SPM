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

using namespace ff;
using namespace std;
static barrierSelector<0> bar;
#define BARRIER   bar.doBarrier(get_my_id())

std::mutex guarder;
int level = 0;
int nw;
int occurancesX;
int s;
int X;
int num_nodes = 100;
int min_edges_per_node = 1;
int max_edges_per_node = 5;
int sum = 1;
vector<bool> processed;
Node<int> source;
Graph<int> graph;
template <class T>
struct params
{
	Graph<T>* graph;
	int source;
	int threadNumber;
	vector<bool>* inQueueOnce;
	vector< vector< queue< Node<T> > > >* levelQueues;
	int* X;
	int level;
    vector<bool>* processed;
	params(Graph<T>* graph, const int& source, const int& threadNumber, vector<bool>* inQueueOnce, vector< vector< queue< Node<T> > > >* levelQueues, int* X, int level, vector<bool>* processed)
	{
		this -> graph = graph;
		this -> source = source;
		this -> threadNumber = threadNumber;
		this -> inQueueOnce = inQueueOnce;
		this -> levelQueues = levelQueues;
		this -> X = X;
        this -> level = level;
        this -> processed = processed;
	}
};

void init_bfs(Graph<int>* graph, vector<Edge>* init_edges, vector<bool>* inQueueOnce, vector< vector< queue< Node<int> > > >* queues) {
    int n = (*init_edges).size();
    for (int i = 0; i < n; i++) {
        if (!(*inQueueOnce)[(*init_edges)[i].getDestID()]) {
            (*queues)[level][i % nw].push((*graph).getNode((*init_edges)[i].getDestID()));
            (*inQueueOnce)[(*init_edges)[i].getDestID()] = true;
        }
        
    }
}

class Worker:public ff_node {
public:
    Worker(void* p ): p(p) {}
    int run(bool=false)  { return ff_node::run();}
    int wait()           { return ff_node::wait();}
    void* setp(void* p) {this->p = p;}
    void set_id(ssize_t id) {
        ff_node::set_id(id);
    }
    void *svc(void*) {
	    params<int> parameters = *((params<int>*)p);
        queue< Node<int> >& toVisit = ((*(parameters.levelQueues))[parameters.level][get_my_id()]);
        int nextLevel = parameters.level ? 0 : 1;
        queue< Node<int> >& toVisitNext = ((*(parameters.levelQueues))[nextLevel][get_my_id()]);
        bool done = false;
        int localX = 0;
        while (!done) {
            //guarder.lock();
            while (!toVisit.empty()) {
                guarder.lock();
                Node<int> current = toVisit.front();
                toVisit.pop();
                guarder.unlock();
                if (current.getVal() == (*(parameters.X))) {
                    localX ++;
                    
                }
                (*(parameters.processed))[current.getNodeID()] = true;
                vector<Edge> outboundEdges = current.getOutboundEdges();
                guarder.lock();
                for (int i = 0; i < outboundEdges.size(); ++i) {
                    if (!((*(parameters.inQueueOnce))[outboundEdges[i].getDestID()])) {
                        guarder.lock();
                        (*(parameters.inQueueOnce))[outboundEdges[i].getDestID()] = true;
                        guarder.unlock();
                        toVisitNext.push(parameters.graph -> getNode(outboundEdges[i].getDestID()));
                    }
                }
                
                
            }
            done = true;
            for (int i = 0; i < nw; ++i) {
                guarder.unlock();
                done |= (!(*(parameters.levelQueues))[parameters.level][i].empty());
                 guarder.unlock();
            }
            
            if (!done) {
                for (int i = 0; i < nw; ++i) {
                    if ((*(parameters.levelQueues))[parameters.level][i].size() > 1) {
                        guarder.lock();
                        toVisit.push((*(parameters.levelQueues))[parameters.level][i].front());
                        (*(parameters.levelQueues))[parameters.level][i].pop();
                        guarder.unlock();
                        break;
                    }
                }
            }
            
        }
        guarder.lock();
        occurancesX += localX;
        guarder.unlock();
        BARRIER;
        return 0;
    }
protected:
    void* p;
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
    source = graph.getNode(s);
    vector< vector< queue< Node<int> > > > queues(2, vector< queue< Node<int> > >(nw));
    vector<Edge> init_edges = source.getOutboundEdges();
    vector<bool> inQueueOnce(num_nodes);
    vector<bool> processed(num_nodes);
    inQueueOnce[s] = true;
    processed[s] = true;
    params<int> p(&graph, s, 0, &inQueueOnce, &queues, &X, level, &processed);

    {   utimer tpg("process init queues");
        init_bfs(&graph, &init_edges, &inQueueOnce, &queues);
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
            N[i]= new Worker((void*)&p);
            //printf("Created thread with %d id \n", i);
            N[i]->set_id(i);
        }
        while(!all_of(processed.begin(), processed.end(), [](bool v) { return v; })){
            //printf("Iteration number %d and level is %d \n", iter, p.level);
            for(int i=0;i<nw;++i) {
                // printf("Before run %d id \n", i);
                N[i]->run();
                // printf("After run %d id \n", i);
            }
            for(int i=0;i<nw;++i) {
                // printf("Before wait %d id \n", i);
                N[i]->wait();
                // printf("After wait %d id \n", i);
            }
            level = level ? 0 : 1;
            iter++;
            p.level = level;
        }
        
    }
    printf ("The number of instances of:: %d is %d \n",  X, occurancesX);
    return 0;
}

