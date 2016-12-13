#include "Graph.h"

#include <limits>

Vertex Graph::detectBubble(const Vertex& start, const int maxDistance) {
    if(start.numOutgoingEdges < 2) return nullptr;

    std::map<Vertex, int> distanceToStart;
    std::map<Vertex, int> unvisitedIncoming;

    for(Vertex v:vertices) {
        distanceToStart[v] = -1;
        unvisitedIncoming[v] = v.numIncomingEdges;
    }

    distanceToStart[start] = 0;

    std::vector<Vertex> S;
    S.push_back(start);

    p = 0;
    while(S.size() > 0) {
        Vertex v = S.pop_back();
        for(Edge e: edges) {
            if(e.end == start) return nullptr;

            if(distanceToStart[e.start] + e.weight > maxDistance) return nullptr;

            if(distanceToStart[e.end] == -1) ++p;

            auto tmp = distanceToStart[e.start] + e.weight;
            if(tmp < distanceToStart[e.end]) {
                distanceToStart[e.end] = tmp;
            }
            --unvisitedIncoming[e.end];

            if(unvisitedIncoming[e.end] == 0) {
                if(e.end.numOutgoingEdges != 0) S.push_back(e.end);
                --p;
            }
        }
        if(S.size() == 1 && p == 0) return S.pop_back();

    }


    return nullptr;
}

