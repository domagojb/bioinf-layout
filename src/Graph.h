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

typedef std::map<Vertex, std::vector<Edge>> Graph;

Vertex invertVertex( const Vertex vertex );

Vertex bVertex( const Edge edge );

size_t edgesCount( const Graph & g, const Vertex & vertex );

bool hasSingleEdge( const Graph & g, const Vertex & vertex );

const Edge & firstEdge( const Graph & g, const Vertex & vertex );

