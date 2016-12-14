//
// Created by Domagoj Boros on 04/12/2016.
//

#pragma once

#include <vector>

class Overlap;

void filterInternalMatches(const std::vector<Overlap *>& source, std::vector<Overlap *>& dest, int o = 1000, float r = 0.8);
void filterContained(const std::vector<Overlap *>& contained, std::vector<Overlap *>& noncontained);

