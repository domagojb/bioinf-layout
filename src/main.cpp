#include <iostream>
#include "common.h"
#include "Overlap.h"
#include "Read.h"
#include "params.h"
#include "ReadTrim.h"
#include "Graph.h"
#include "OverlapUtils.h"
#include "IO.h"
#include "GraphUtils.h"
#include "Unitig.h"
#include "UnitigUtils.h"


int main() {

    Overlaps overlaps;
    Reads    reads;
    Params   params( getDefaultParams());

    std::cout << "1) Reading overlaps and reads" << std::endl;
    loadPAF( overlaps, reads, "../test-data/lambda_overlaps.paf", params );

    std::cout << "2) Proposing read trims" << std::endl;
    ReadTrims readTrims;
    proposeReadTrims( readTrims, overlaps, params, false );

    std::cout << "3) Trimming reads" << std::endl;
    trimReads( overlaps, readTrims, params );

    std::cout << "3) Filtering reads" << std::endl;
    filterReads( overlaps, readTrims, params );

    std::cout << "4) Proposing read trims" << std::endl;
    ReadTrims readTrims2;
    proposeReadTrims( readTrims2, overlaps, params, true );

    std::cout << "5) Trimming reads" << std::endl;
    trimReads( overlaps, readTrims2, params );

    mergeTrims( readTrims, readTrims2 );

    std::cout << "6) Chimering reads" << std::endl;
    filterChimeric( overlaps, readTrims, params );

    std::cout << "7) Filtering contained reads" << std::endl;
    filterContained( overlaps, readTrims, params );

    //    logTrimmedOverlaps( overlaps, readTrims );

    std::cout << "8) Generating graph" << std::endl;
    Graph g;
    generateGraph( g, overlaps, readTrims, params );

    //    logGraph(g);

    std::cout << "9) Filtering transitive edges" << std::endl;
    filterTransitiveEdges( g, 1000 );

    //    logGraph(g);

    std::cout << "10) Removing asymetric edges" << std::endl;
    removeAsymetricEdges( g );

    std::cout << "11) Cutting tips" << std::endl;
    cutTips( g, readTrims, params );

    writeGraphToSIF( "../test-data/notips.sif", g );

    std::cout << "12) Popping bubbles" << std::endl;
    popBubbles( g, readTrims );

    writeGraphToSIF( "../test-data/nobubles.sif", g );

    //    logGraph(g);

    Unitigs unitigs;
    std::cout << "13) Generating unitigs" << std::endl;
    generateUnitigs( unitigs, g, readTrims );

    assignSequencesToUnitigs( unitigs, readTrims, "../test-data/lambda_reads.fasta" );

    logUnitigs( unitigs, readTrims );


    //    logGraph(g);

    //    std::cout << "Left with " << overlaps.size() << " trimmed overlaps" << std::endl;

    //
    //    for (auto const &readTrim: readTrims) {
    //        std::cout << readTrim.first << " " << readTrim.second.toString() << std::endl;
    //    }



    //    writeOverlapsToSIF("mnebijemte.sif", overlaps);


    return 0;
}
