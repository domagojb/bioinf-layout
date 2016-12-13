#include <vector>
#include <map>

class Vertex {
public:

    int numIncomingEdges;
    int numOutgoingEdges;
};

class Edge {
public:
    Vertex start;
    Vertex end;
    int weight;

};

class Graph {
public:

    Vertex detectBubble(const Vertex& start, const int maxDistance);

    std::vector<Edge> edges;

    std::vector<Vertex> vertices;

    std::map<Vertex, std::vector<Edge>> vertexEdgeMap;

};
