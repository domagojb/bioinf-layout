#include "Graph.h"

Vertex invertVertex( const Vertex vertex ) {
    return std::make_pair( vertex.first, !vertex.second );
}

Vertex bVertex( const Edge edge ) {
    return std::make_pair( edge.bId, edge.bIsReversed );
}

size_t edgesCount( const Graph & g, const Vertex & vertex ) {
    return g.at( vertex ).size();
}

bool hasSingleEdge( const Graph & g, const Vertex & vertex ) {
    return edgesCount( g, vertex ) == 1;
}

const Edge & firstEdge( const Graph & g, const Vertex & vertex ) {
    return g.at( vertex ).front();
}
