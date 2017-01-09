#pragma once

#include <cstddef>
#include <utility>
#include <vector>
#include <unordered_map>
#include <map>
#include "common.h"

class Edge {

public:
    read_size_t aId;
    read_size_t bId;
    bool  aIsReversed;
    bool  bIsReversed;
    read_size_t overlapLength;
    bool  del;
    read_size_t numberOfSequenceMatches;
    float numberOfSequenceMatchesRatio;
    bool  visited = false;
};


typedef std::pair<read_id_t, bool> Vertex;

struct VertexHash {
public:
    std::size_t operator()( const Vertex & x ) const {
        return x.first << 1 | static_cast<std::size_t >(x.second);
    }
};

typedef std::vector<Edge> Edges;
typedef std::map<Vertex, Edges> Graph;

inline Vertex makeVertex( const read_id_t id, const bool isReversed) { return std::make_pair( id, isReversed ); }

inline Vertex invertVertex( const Vertex vertex ) { return makeVertex( vertex.first, !vertex.second ); }

inline Vertex bVertex( const Edge edge ) { return makeVertex( edge.bId, edge.bIsReversed ); }

inline Vertex bInvertVertex( const Edge edge ) { return invertVertex(bVertex(edge)); }

inline size_t edgesCount( const Graph & g, const Vertex & vertex ) { return g.at( vertex ).size(); }

inline bool hasSingleEdge( const Graph & g, const Vertex & vertex ) { return edgesCount( g, vertex ) == 1; }

inline const Edge & firstEdge( const Graph & g, const Vertex & vertex ) { return g.at( vertex ).front(); }