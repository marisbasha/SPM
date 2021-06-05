#include <latch>
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

using namespace std;

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
int processed;


template <class T>
struct params
{
	Graph<T>* graph;
	int source;
	vector<bool>* inQueueOnce;
	vector< vector< queue< Node<T> > > >* levelQueues;
	int* X;
	int level;
    vector<bool>* processed;
	params(Graph<T>* graph, const int& source, vector<bool>* inQueueOnce, vector< vector< queue< Node<T> > > >* levelQueues, int* X, int level, vector<bool>* processed)
	{
		this -> graph = graph;
		this -> source = source;
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
            (*queues)[level][nw > 1 ? i % nw : 0].push((*graph).getNode((*init_edges)[i].getDestID()));
            (*inQueueOnce)[(*init_edges)[i].getDestID()] = true;
        }
    }
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
    Graph<int> graph;
    {   utimer tpg("process graph");
        processGraph(&graph, filename);
    }
    Node<int> source = graph.getNode(s);
    vector< vector< queue< Node<int> > > > queues(2, vector< queue< Node<int> > >(nw));
    vector<Edge> init_edges = source.getOutboundEdges();
    vector<bool> inQueueOnce(num_nodes);
    vector<bool> processed(num_nodes);
    inQueueOnce[s] = true;
    processed[s] = true;
    params<int> p(&graph, s, &inQueueOnce, &queues, &X, level, &processed);
    {   utimer tpg("process init queues");
        init_bfs(&graph, &init_edges, &inQueueOnce, &queues);
    }
    latch work_done{nw};
    vector<thread> threads;
    auto bfs = [&](void* p, int t) {
        params<int> parameters = *((params<int>*)p);
        queue< Node<int> >& toVisit = ((*(parameters.levelQueues))[parameters.level][t]);
        int nextLevel = parameters.level ? 0 : 1;
        queue< Node<int> >& toVisitNext = ((*(parameters.levelQueues))[nextLevel][t]);
        bool done = false;
        int localX = 0;
        while (!done) {
            guarder.lock();
            while (!toVisit.empty()) {
                Node<int> current = toVisit.front();
                toVisit.pop();
                (*(parameters.processed))[current.getNodeID()] = true;
                if (current.getVal() == (*(parameters.X))) {
                    localX ++;
                }
                vector<Edge> outboundEdges = current.getOutboundEdges();
                
                for (int i = 0; i < outboundEdges.size(); ++i) {
                    
                    if (!((*(parameters.inQueueOnce))[outboundEdges[i].getDestID()])) {
                        (*(parameters.inQueueOnce))[outboundEdges[i].getDestID()] = true;
                        toVisitNext.push(parameters.graph -> getNode(outboundEdges[i].getDestID()));
                    }
                }
                
            }
            done = true;
            for (int i = 0; i < nw; ++i) {
                done |= (!(*(parameters.levelQueues))[parameters.level][i].empty());
            }
            
            if (!done) {
                for (int i = 0; i < nw; ++i) {
                    if ((*(parameters.levelQueues))[parameters.level][i].size() > 1) {
                        toVisit.push((*(parameters.levelQueues))[parameters.level][i].front());
                        (*(parameters.levelQueues))[parameters.level][i].pop();
                        break;
                    }
                }
            }
            
            guarder.unlock();
        }
        guarder.lock();
        occurancesX += localX;
        guarder.unlock();
        work_done.count_down();
    };
    {   utimer tpg("process parallel bfs");
        int iter;    
        
        
      
        while(!all_of(processed.begin(), processed.end(), [](bool v) { return v; })){
            
            
            for(int i=0;i<nw;++i) {
                threads.emplace_back(bfs, &p, i);
            }
            work_done.wait();
            for (auto& thread : threads) {
                thread.join();
            }
            threads.erase(threads.begin(), threads.end());
            work_done.~latch();
            latch work_done{nw};
            level = level ? 0 : 1;
            p.level = level;
            iter++;            
        }
        
        
    }
    printf ("The number of instances of:: %d is %d \n",  X, occurancesX);
    return 0;
}