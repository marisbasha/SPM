#ifndef BFS
#define BFS
#include <vector>
#include <queue>

mutex currentQueueGuarder;
mutex checkCompletionGuarder;
mutex checkProcessLock;

void bfs(   
    int tid, 
    int nw,
    int& X,
    int& occurancesX,
    Graph<int>& graph,
    vector<queue< Node<int> >>&firstQueues, 
    vector<queue< Node<int> >>&secondQueues,
    vector<bool>& processed,
    MultilevelBarrier& work_done

) {
    queue< Node<int> > currentQueue = firstQueues[tid];
    queue< Node<int> > nextQueue = secondQueues[tid];
    Node<int> current;
    bool done = false;
    int localX = 0;
    while (!done) {
        while (!currentQueue.empty()) {
#ifdef WITHTIME 
    {   utimer tpg("Time to pop queue with mutex");
#endif
            currentQueueGuarder.lock();
            current = currentQueue.front();
            currentQueue.pop();
            currentQueueGuarder.unlock();
            if (current.getVal() == X) localX ++;
#ifdef WITHTIME 
    }
#endif
            
#ifdef WAIT
            usleep(5000); // We assume we make work on the current element being processed
#endif
            vector<Edge> outboundEdges = current.getOutboundEdges();
#ifdef WITHTIME 
    {   utimer tpg("Time to process edges with lock");
#endif
            for (long unsigned int i = 0; i < outboundEdges.size(); ++i) {
                if (!processed[outboundEdges[i].getDestID()]) {
                    checkProcessLock.lock();
                    processed[outboundEdges[i].getDestID()] = true;
                    checkProcessLock.unlock();
                    nextQueue.push(graph.getNode(outboundEdges[i].getDestID()));
                }
            }
#ifdef WITHTIME 
    }
#endif
        }
        done = true;
        for (int i = 0; i < nw; ++i) {
            done &= !firstQueues[i].empty();
            if(!done && firstQueues[i].size() > 1) {
                currentQueueGuarder.lock();
                currentQueue.push(firstQueues[i].front());
                firstQueues[i].pop();
                currentQueueGuarder.unlock();
                break;
            }
        }
#ifdef WITHTIME 
    {   utimer tpg("Time to swap queues and barrier");
#endif
        if (done) {
            work_done.count_down_and_wait(); // Barrier to wait next iteration
            queue<Node<int>> t = currentQueue;
            currentQueue = nextQueue;
            nextQueue = t;
            work_done.count_down_and_wait(); // Barrier to wait next iteration
            if(!currentQueue.empty()) done = false; // Continue if there's still work
        
        }
#ifdef WITHTIME 
    }
#endif
        if(done) work_done.decrease_active_threads();
    }
#ifdef WITHTIME 
    {   utimer tpg("Time to update x with locks");
#endif
    checkCompletionGuarder.lock();
    occurancesX += localX;
    checkCompletionGuarder.unlock();
#ifdef WITHTIME 
    }
#endif
}

#endif