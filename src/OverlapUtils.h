//
// Created by Domagoj Boros on 04/12/2016.
//

#pragma once

#include <vector>
#include "Graph.h"

class Overlap;

void filterInternalMatches(const std::vector<Overlap *>& source, std::vector<Overlap *>& dest, int o = 1000, float r = 0.8);
void filterContained(const std::vector<Overlap *>& contained, std::vector<Overlap *>& noncontained);
void filterTransitiveEdges(const std::vector<Overlap *>& nonContained, std::vector<Overlap *>& nonTransitive);


//void generateAssemblyGraph(std::vector<Overlap *> vector, Graph& graph);
