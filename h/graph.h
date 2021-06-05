#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <iostream>
#include <cstdlib>

#include "edge.h"
#include "node.h"

template <class NodeT>
class Graph {
    private:
        std::vector<Node<NodeT>> nodes;
        
    public:
        Graph(); // Creates an empty Graph
        Graph(const std::vector<Node<NodeT>>& nodes); // Creates a graph using the specified list of Nodes
        
        Graph& addNode(const Node<NodeT>& node); // Adds a Node Object to the Graph and assigns it the next available NodeID
        Graph& addNode(const long unsigned int& nodeID, const NodeT& data); // Adds a Node with specified data and assigns it the next available NodeID
        Graph& addNode(const NodeT& data); // Adds a Node with specified data and assigns it the next available NodeID
        
        Graph& addEdge(const long unsigned int& sourceID, const Edge& edge); // Adds an Edge Object to the Graph at the node specified by sourceID
        Graph& addEdge(const long unsigned int& sourceID, const long unsigned int& destID); // Adds an Edge from source node to dest node
        
        Node<NodeT> getNode(const long unsigned int& nodeID) const; // Returns the specified Node from the Node List
        long unsigned int getNumberOfNodes() const; // Returns the number of nodes in the graph
};

template <class NodeT>
Graph<NodeT>::Graph() { }

template <class NodeT>
Graph<NodeT>::Graph(const std::vector<Node<NodeT>>& nodes) {
	this->nodes = nodes;
}

template <class NodeT>
Graph<NodeT>& Graph<NodeT>::addNode(const Node<NodeT>& node) {
	nodes.push_back(node);
	return (*this);
}

template <class NodeT>
Graph<NodeT>& Graph<NodeT>::addNode(const NodeT& data) {
	nodes.push_back(Node<NodeT>(nodes.size(), data));
	return (*this);
}

template <class NodeT>
Graph<NodeT>& Graph<NodeT>::addNode(const long unsigned int& nodeID, const NodeT& data) {
	nodes.push_back(Node<NodeT>(nodeID, data));
	return (*this);
}

template <class NodeT>
Graph<NodeT>& Graph<NodeT>::addEdge(const long unsigned int& sourceID, const Edge& edge) {
	if (sourceID >= nodes.size()) {
		std::cout << "ERROR: Specified sourceID is not in the Graph." << std::endl;
		exit(EXIT_FAILURE);
	}
	nodes[sourceID].edges.push_back(edge);
	return (*this);
}

template <class NodeT>
Graph<NodeT>& Graph<NodeT>::addEdge(const long unsigned int& sourceID, const long unsigned int& destID) {
	if (sourceID >= nodes.size()) {
		std::cout << "ERROR: Specified sourceID is not in the Graph." << std::endl;
		exit(EXIT_FAILURE);
	}
	nodes[sourceID].addOutboundEdge(destID);	
	return (*this);
}

template <class NodeT>
Node<NodeT> Graph<NodeT>::getNode(const long unsigned int& nodeID) const {
	if (nodeID >= nodes.size()) {
		std::cout << "ERROR: Specified nodeID is not in the Graph." << std::endl;
		exit(EXIT_FAILURE);
	}
	return nodes[nodeID];
}

template <class NodeT>
long unsigned int Graph<NodeT>::getNumberOfNodes() const {
	return nodes.size();
}

#endif // GRAPH_H