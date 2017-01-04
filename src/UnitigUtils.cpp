//
// Created by Ivan Jurin on 1/4/17.
//
#include "UnitigUtils.h"

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
            if ( !hasSingleEdge( g, vertexCurrent )) break;

            Vertex vertexNext( bVertex( firstEdge( g, vertexCurrent )));


            if ( !hasSingleEdge( g, invertVertex( vertexNext ))) break;

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
                if ( !hasSingleEdge( g, invertVertex( vertexNext ))) break;

                vertexCurrent = invertVertex( bVertex( firstEdge( g, invertVertex( vertexNext ))));

                if ( !hasSingleEdge( g, vertexCurrent )) break;

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

        unitigs.emplace_back( vertexStart, vertexEnd, len, !isLinearUnitig, unitigReads );

    }

    // todo: joining unitigs
}
