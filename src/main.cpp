#include <iostream>
#include <unordered_set>
#include <queue>

#include "common.h"

#include "IO.h"
#include "OverlapUtils.h"
#include "GraphUtils.h"
#include "Unitig.h"


void logUnitigs( const Unitigs & unitigs, const ReadTrims & readTrims ) {
    auto   fp = stdout;
    size_t i( 0 );
    char   name[32];
    for ( Unitig const & unitig : unitigs ) {
        sprintf( name, "utg%.6ld%c", i++ + 1, "lc"[unitig.isCircular] );
        fprintf( fp,
                 "S\t%s\t%s\tLN:i:%d\n",
                 name,
                 !unitig.sequence.empty() ? unitig.sequence.c_str() : "*",
                 unitig.length
               );


        read_size_t cumLength( 0 );
        for ( UnitigRead const & unitigRead : unitig.reads ) {
            auto const & vertex( unitigRead.first );
            read_size_t length( unitigRead.second );

            auto readId( vertex.first );
            auto isReversed( vertex.second );

            auto const & readTrim( readTrims.at( readId ));

            fprintf( fp,
                     "a\t%s\t%d\t%d:%d-%d\t%c\t%d\n",
                     name,
                     cumLength,
                     readId,
                     readTrim.start + 1,
                     readTrim.end,
                     "+-"[isReversed],
                     length
                   );


            cumLength += length;
        }

    }
    // todo: print joined unitigs
    // todo: print unitig summaries
}

void generateUnitigs( Unitigs & unitigs, Graph const & g, ReadTrims const & readTrims ) {
    //#define printv(name, v) name<<" "<<(v).first<<" !"[(v).second]

    UnitigReads unitigReads;

    std::unordered_set<Vertex, VertexHash> hasVisited;

    for ( auto const & vertexOutgoingEdges: g ) {
        Vertex const & vertex( vertexOutgoingEdges.first );

        if ( readTrims.at( vertex.first ).del ) continue;

        auto const & edges( vertexOutgoingEdges.second );

        if ( edges.size() == 0 ) continue;


        if ( hasVisited.find( vertex ) != hasVisited.end()) continue;


        hasVisited.insert( vertex );

        Vertex vertexCurrent( vertex );
        Vertex vertexStart( vertex );
        Vertex vertexEnd( invertVertex( vertex ));

        read_size_t len( 0 );

        while ( true ) {
            if ( !hasSingleEdge(g, vertexCurrent ) ) break;

            Vertex vertexNext( bVertex(firstEdge(g,vertexCurrent)));


            if ( !hasSingleEdge( g, invertVertex( vertexNext )) ) break;

            hasVisited.insert( vertexNext );
            hasVisited.insert( invertVertex( vertexCurrent ));


            read_size_t l( firstEdge( g, vertexCurrent ).overlapLength );


            unitigReads.emplace_back( vertexCurrent, l );
            len += l;

            vertexEnd = invertVertex( vertexNext );

            vertexCurrent = vertexNext;

            if ( vertexNext == vertex ) break;
        }

        bool isLinearUnitig( vertexStart != invertVertex( vertexEnd ) || unitigReads.size() == 0 );

        if ( isLinearUnitig ) { // linear unitig
            read_size_t l = readTrims.at( vertexEnd.first ).length();
            len += l;
            unitigReads.emplace_back( invertVertex( vertexEnd ), l );

            Vertex vertexNext = vertex;

            while ( true ) {
                if ( !hasSingleEdge( g, invertVertex( vertexNext )) ) break;

                vertexCurrent = invertVertex( bVertex( firstEdge( g, invertVertex( vertexNext ))));

                if ( !hasSingleEdge(g, vertexCurrent ) ) break;

                hasVisited.insert( vertexNext );
                hasVisited.insert( invertVertex( vertexCurrent ));

                l = firstEdge( g, vertexCurrent ).overlapLength;
                unitigReads.emplace_front( vertexCurrent, l );

                vertexStart = vertexCurrent;
                len += l;
                vertexNext  = vertexCurrent;
            }

            hasVisited.insert( vertexStart );
            hasVisited.insert( vertexEnd );
        }

        unitigs.emplace_back(vertexStart, vertexEnd, len, !isLinearUnitig, unitigReads);

    }

    // todo: joining unitigs
}


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

    //
    //    for (auto &pair : readTrims) {
    //        std::cout<<pair.first+100000<<" "<<pair.second.toString()<<std::endl;
    //    }
    //exit(0);


//    logTrimmedOverlaps( overlaps, readTrims );

    std::cout << "8) Generating graph" << std::endl;
    Graph g;
    generateGraph( g, overlaps, readTrims, params );

    //    logGraph(g);

    filterTransitiveEdges( g, 1000 );

    //    logGraph(g);

    removeAsymetricEdges( g );

    //    cleanGraph(g);


    cutTips( g, readTrims, params );

    writeGraphToSIF( "../test-data/notips.sif", g );


    popBubbles( g, readTrims );

    writeGraphToSIF( "../test-data/nobubles.sif", g );


//    logGraph(g);

    Unitigs unitigs;
    generateUnitigs( unitigs, g, readTrims );

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
