//
// Created by Domagoj Boros on 18/12/2016.
//

#include "OverlapUtils.h"

#include "GraphUtils.h"

void generateGraph(Graph &g, const Overlaps &overlaps, const ReadTrims& readTrims, Params &params) {

    for (const auto& o : overlaps) {
        OverlapClassification c;
        Edge e;
        classifyOverlapAndMeasureItsLength(c, e, o, readTrims.at(o.aId()).length(), readTrims.at(o.bId()).length(), params.maximalOverhangLength, params.mappingLengthRatio, params.minimalOverlap);
        if (c == OVERLAP_A_TO_B || c == OVERLAP_B_TO_A) {
            g.push_back(e);
        }
    }

    std::cout << "Generated " << g.size() << " edges" << std::endl;
}
