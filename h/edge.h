#ifndef EDGE_H
#define EDGE_H

class Edge {
    private:
        int destID;
        
    public:
        Edge(); // Creates Edge with 0 destID
        Edge(const int& destID); // Creates Edge with specified destID
        
        int getDestID(); // Returns the destination ID of the edge
};

Edge::Edge() {
	this -> destID = 0;
}

Edge::Edge(const int& destID) {
	this->destID = destID;
}

int Edge::getDestID() {
	return this->destID;
}
#endif // EDGE_H