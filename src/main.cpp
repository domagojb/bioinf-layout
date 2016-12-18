#include <iostream>

#include "common.h"

#include "IO.h"
#include "OverlapUtils.h"
#include "GraphUtils.h"

int main() {

    Overlaps overlaps;
    Reads reads;
    Params params(getDefaultParams());

    std::cout << "1) Reading overlaps and reads" << std::endl;
    loadPAF(overlaps, reads, "../test-data/lambda_overlaps.paf", params);

    std::cout << "2) Proposing read trims" << std::endl;
    ReadTrims readTrims;
    proposeReadTrims(readTrims, overlaps, params, false);

    std::cout << "3) Trimming reads" << std::endl;
    trimReads(overlaps, readTrims, params);

    std::cout << "3) Filtering reads" << std::endl;
    filterReads(overlaps, readTrims, params);

    std::cout << "4) Proposing read trims" << std::endl;
    ReadTrims readTrims2;
    proposeReadTrims(readTrims2, overlaps, params, true);

    std::cout << "5) Trimming reads" << std::endl;
    trimReads(overlaps, readTrims2, params);

    mergeTrims(readTrims,readTrims2);


    std::cout << "6) Chimering reads" << std::endl;
    filterChimeric(overlaps,readTrims,params);

    std::cout << "7) Filtering contained reads" << std::endl;
    filterContained(overlaps,readTrims,params);

//
//    for (auto &pair : readTrims) {
//        std::cout<<pair.first+100000<<" "<<pair.second.toString()<<std::endl;
//    }
//exit(0);


    logTrimmedOverlaps(overlaps, readTrims);

    std::cout << "8) Generating graph" << std::endl;
    Graph g;
    generateGraph(g, overlaps, readTrims, params);

    filterTransitiveEdges(g, 1000);

//    std::cout << "Left with " << overlaps.size() << " trimmed overlaps" << std::endl;

//
//    for (auto const &readTrim: readTrims) {
//        std::cout << readTrim.first << " " << readTrim.second.toString() << std::endl;
//    }



//    writeOverlapsToSIF("mnebijemte.sif", overlaps);


    return 0;
}
