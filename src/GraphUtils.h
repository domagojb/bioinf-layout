//
// Created by Domagoj Boros on 18/12/2016.
//

#pragma once

#include "Graph.h"

void generateGraph(Graph &g, const Overlaps &overlaps, const ReadTrims& readTrims, Params &params);

void filterTransitiveEdges(Graph& g, read_size_t FUZZ);

void removeAsymetricEdges(Graph& g);

void logGraph(const Graph & g );