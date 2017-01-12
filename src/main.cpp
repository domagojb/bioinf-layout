#include <iostream>
#include "Overlap.h"
#include "params.h"
#include "ReadTrim.h"
#include "Graph.h"
#include "OverlapUtils.h"
#include "IO.h"
#include "GraphUtils.h"
#include "UnitigUtils.h"
#include "dotter.h"

Unitigs runAlgorithm(const std::string & overlapsPath, const std::string & readsPath){
    TIMER_START("Algorithm");
    Overlaps overlaps;
    Params   params( getDefaultParams());

    std::cout << "1) Reading overlaps and reads" << std::endl;
    loadPAF( overlaps, overlapsPath, params );

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

    std::cout << "11) Popping bubbles" << std::endl;
    popBubbles( g, readTrims );

    std::cout << "12) Removing short edges" << std::endl;
    for (int i = 0; i <= 2; ++i) {
        float r = params.minOverlapDropRaion + (params.maxOverlapDropRation - params.minOverlapDropRaion) / 2 * i;
        if (deleteShortEdges(g, r)) {
            cutTips( g, readTrims, params );
            popBubbles( g, readTrims );
        }
    }

    std::cout << "13) Generating unitigs" << std::endl;
    Unitigs unitigs;
    generateUnitigs( unitigs, g, readTrims );
    TIMER_END("Algorithm");

    if ( ASSIGN_SEQUENCES_TO_UNITIGS && !readsPath.empty() ) {
        std::cout << "14) Assigning sequences to unitigs" << std::endl;
        assignSequencesToUnitigs( unitigs, readTrims, readsPath );
    }

    logUnitigs(unitigs, readTrims);

    return unitigs;
}

int main(int argc, char *argv[]) {

    if ( argc <= 2 ) return -1;

    // path to .PAF file with overlaps
    std::string overlapsPath( argv[1] );

    // path to .FASTA file with reads for assigning sequences to unitigs [not required]
    std::string readsPath( argc >= 2 ? argv[2] : "" );

    // path to .FASTA file with reference sequence for dotter [not required]
    std::string referenceSequencePath( argc >= 3 ? argv[3] : "" );

    // path to .FASTA file with reference sequence for dotter [not required]
    std::string resultSequencePath( "/tmp/result.fasta" );

    Unitigs unitigs = runAlgorithm( overlapsPath, readsPath );

    if ( referenceSequencePath.empty()) return 0;

    unitigsToFASTA( resultSequencePath, unitigs );
    dotter( resultSequencePath, referenceSequencePath );

    return 0;
}
