#include <iostream>
#include "common.h"
#include "Overlap.h"
#include "params.h"
#include "ReadTrim.h"
#include "Graph.h"
#include "OverlapUtils.h"
#include "IO.h"
#include "GraphUtils.h"
#include "Unitig.h"
#include "UnitigUtils.h"

#define DATASET "ecoli"

int main() {

    Overlaps overlaps;
    Params   params( getDefaultParams());

    std::cout << "1) Reading overlaps and reads" << std::endl;
    loadPAF( overlaps, "../test-data/" DATASET "_overlaps.paf", params );

    std::cout << "2) Proposing read trims" << std::endl;
    ReadTrims readTrims;
    proposeReadTrims( readTrims, overlaps, params );

    std::cout << "3) Trimming reads" << std::endl;
    trimReads( overlaps, readTrims, params );

    std::cout << "4) Filtering internal reads" << std::endl;
    filterInternalReads( overlaps, readTrims, params );

    std::cout << "5) Chimering reads" << std::endl;
    filterChimeric( overlaps, readTrims, params );

    std::cout << "6) Filtering contained reads" << std::endl;
    filterContained( overlaps, readTrims, params );

    std::cout << "7) Generating graph" << std::endl;
    Graph g;
    generateGraph( g, overlaps, readTrims, params );

    std::cout << "8) Filtering transitive edges" << std::endl;
    filterTransitiveEdges( g, params );

    std::cout << "9) Removing asymetric edges" << std::endl;
    removeAsymetricEdges( g );

    std::cout << "10) Cutting tips" << std::endl;
    cutTips( g, readTrims, params );

    writeGraphToSIF( "../test-data/" DATASET "_notips.sif", g );

    std::cout << "11) Popping bubbles" << std::endl;
    popBubbles( g, readTrims );

    writeGraphToSIF( "../test-data/" DATASET "_nobubles.sif", g );

    Unitigs unitigs;
    std::cout << "12) Generating unitigs" << std::endl;
    generateUnitigs( unitigs, g, readTrims );

    assignSequencesToUnitigs( unitigs, readTrims, "../test-data/" DATASET "_reads.fasta" );

    logUnitigs( unitigs, readTrims );

    return 0;
}
