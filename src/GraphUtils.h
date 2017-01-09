//
// Created by Domagoj Boros on 18/12/2016.
//

#pragma once

#include "Graph.h"


void generateGraph( Graph & g, const Overlaps & overlaps,  ReadTrims & readTrims, Params & params );

void filterTransitiveEdges( Graph & g, read_size_t FUZZ );

void removeAsymetricEdges( Graph & g );

void logGraph( const Graph & g, const ReadTrims & readTrims );

void logGraphToFile( std::ofstream & ofstream, const Graph & g, const ReadTrims & readTrims );

void cleanGraph( Graph & g );


enum GraphEdgeType {
    GRAPH_EDGE_TYPE_MERGEABLE = 0,
    GRAPH_EDGE_TYPE_TIP,
    GRAPH_EDGE_TYPE_MULTI_OUT,
    GRAPH_EDGE_TYPE_MULTI_NEI
};


void cutTips( Graph & g, ReadTrims & readTrims, const Params & params );

void popBubbles( Graph & g, ReadTrims & readTrims );