#ifndef NODE_H
#define NODE_H

#include <vector>
#include <algorithm>
#include "edge.h"

template <typename T> bool PComp(const T * const & a, const T * const & b)
{
   return *a == *b;
}

template <class T>
class Node {
    private:
        int nodeID; // ID of the Node
        T val; // Value to store at Node
        std::vector<Edge> edges; // Outbound Edges
        
    public:
        Node(); // Creates Node with empty value and empty edges with -1 nodeID
        Node(const int& nodeID, const T& val); // Creates Node with specified value and empty edges
        Node(const int& nodeID, const T& val, const std::vector<Edge>& edges); // Creates Node with specified value and edges
        Node& addOutboundEdge(const int& destID); // Adds an edge with specified parameters
        int getNodeID(); // Returns the ID of the node
        T getVal(); // Returns the value associated with the Node
        std::vector<Edge> getOutboundEdges(); // Returns the Edges going out of the Node
};

template <class T>
Node<T>::Node() {
	this->nodeID = -1;
}

template <class T>
Node<T>::Node(const int& nodeID, const T& val) {
	this->nodeID = nodeID;
	this->val = val;
}

template <class T>
Node<T>::Node(const int& nodeID, const T& val, const std::vector<Edge>& edges) {
	this->nodeID = nodeID;
	this->val = val;
	this->edges = edges;
}

template <class T>
int Node<T>::getNodeID() {
	return this->nodeID;
}

template <class T>
T Node<T>::getVal() {
	return this->val;
}

template <class T>
std::vector<Edge> Node<T>::getOutboundEdges() {
	return this->edges;
}

template <class T>
Node<T>& Node<T>::addOutboundEdge(const int& destID) {
	edges.push_back(Edge(destID));
	return (*this);
}


#endif // NODE_H