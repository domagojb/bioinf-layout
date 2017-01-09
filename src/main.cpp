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

int count(const Graph & g){
    int cnt =0;
    for ( const auto & pair : g ) {
        for ( const auto & item : pair.second ) {
            if(!item.del){
                cnt++;
            }
        }
    }
    return cnt;
}

bool deleteShort( Graph g, ReadTrims readTrims, float r ) {
    int n_short = 0;
    for(auto & pair : g){
        int max_ml = 0;

        if(pair.second.size()<2) continue;

        for ( auto & edge : pair.second ) {
            max_ml = std::max(max_ml,edge.numberOfSequenceMatches);
        }
        if(max_ml != pair.second[0].numberOfSequenceMatches) continue;
        int thres = pair.second[0].numberOfSequenceMatches * r + .499;
        int i;
        for (i = pair.second.size() - 1; i >= 1 && pair.second[i].numberOfSequenceMatches< thres; --i);
        for (i = i + 1; i <  pair.second.size(); ++i)
            pair.second[i].del = 1, ++n_short;
    }

    cleanGraph(g);
    fprintf(stderr, "[M::%s] removed %d short overlaps\n", __func__, n_short);
    return n_short!=0;
}

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

    std::cout << "4) Filtering reads" << std::endl;
    filterReads( overlaps, readTrims, params );

    std::cout << "5) Chimering reads" << std::endl;
    filterChimeric( overlaps, readTrims, params );

    std::cout << "6) Filtering contained reads" << std::endl;
    filterContained( overlaps, readTrims, params );

    std::cout << "7) Generating graph" << std::endl;
    Graph g;
    generateGraph( g, overlaps, readTrims, params );

    std::cout << "8) Filtering transitive edges" << std::endl;
    filterTransitiveEdges( g, 1000 );

    std::cout << "9) Removing asymetric edges" << std::endl;
    removeAsymetricEdges( g );



    std::cout << "10) Cutting tips" << std::endl;
    cutTips( g, readTrims, params );



    writeGraphToSIF( "../test-data/" DATASET "_notips.sif", g );

    std::cout << "11) Popping bubbles" << std::endl;
    popBubbles( g, readTrims );

    for (int i = 0; i <= params.n_rounds; ++i) {
        float r = params.min_ovlp_drop_ratio + (params.max_ovlp_drop_ratio - params.min_ovlp_drop_ratio) / params.n_rounds * i;

        if(deleteShort( g, readTrims, r )) {
            cutTips( g, readTrims, params );
            popBubbles( g, readTrims );
        }
    }

    writeGraphToSIF( "../test-data/" DATASET "_nobubles.sif", g );
//
//    logGraph(g,readTrims);
//    exit(0);


    Unitigs unitigs;
    std::cout << "12) Generating unitigs" << std::endl;
    generateUnitigs( unitigs, g, readTrims );

    assignSequencesToUnitigs( unitigs, readTrims, "../test-data/" DATASET "_reads.fasta" );

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
