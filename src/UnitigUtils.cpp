//
// Created by Ivan Jurin on 1/4/17.
//
#include <fstream>
#include <assert.h>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include "UnitigUtils.h"

void generateUnitigs( Unitigs & unitigs, Graph const & g, ReadTrims const & readTrims ) {
    TIMER_START("Generating unitigs...");
    //#define printv(name, v) name<<" "<<(v).first<<" !"[(v).second]

    UnitigReads unitigReads;

    std::unordered_set<Vertex, VertexHash> hasVisited;

    for ( auto const & vertexOutgoingEdges: g ) {
        Vertex const & vertex( vertexOutgoingEdges.first );

        if ( readTrims.at( vertex.first ).del ) continue;

        auto const & edges( vertexOutgoingEdges.second );

        if ( edges.size() == 0 ) continue;


        if ( hasVisited.find( vertex ) != hasVisited.end()) continue;

        unitigReads.clear();

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

    std::cout << "Generated " << unitigs.size() << " unitig" << " s"[unitigs.size() > 1] << std::endl;
    TIMER_END("Done with unitigs, time elapsed: ");
}


// borrowed from original miniasm implementation
static char comp_tab[] = { // complement base
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
        58, 59, 60, 61, 62, 63, 64, 'T', 'V', 'G', 'H', 'E', 'F', 'C', 'D', 'I', 'J', 'M', 'L', 'K', 'N', 'O', 'P', 'Q',
        'Y', 'S', 'A', 'A', 'B', 'W', 'X', 'R', 'Z', 91, 92, 93, 94, 95, 64, 't', 'v', 'g', 'h', 'e', 'f', 'c', 'd',
        'i', 'j', 'm', 'l', 'k', 'n', 'o', 'p', 'q', 'y', 's', 'a', 'a', 'b', 'w', 'x', 'r', 'z', 123, 124, 125, 126,
        127 };

void assignSequencesToUnitigs( Unitigs & unitigs, const ReadTrims & readTrims, const std::string pathToFASTA ) {
    TIMER_START(__func__);

    std::unordered_map<read_id_t, std::string> sequences;

    std::ifstream is;
    is.open( pathToFASTA );
    auto        expectSequence( false );
    read_id_t   expectedReadId( 0 );
    std::string line;
    while ( std::getline( is, line )) {
        if ( expectSequence ) {
            auto it( readTrims.find( expectedReadId ));
            if ( it != readTrims.end()) {
                auto const & readTrim( it->second );
                sequences.emplace( expectedReadId, line.substr((size_t) readTrim.start, (size_t) readTrim.length()));
            }
            expectSequence = false;
        } else if ( !line.empty() && line[0] == '>' ) {
            std::istringstream iss( line.substr( 1 ));
            if ( !( iss >> expectedReadId )) assert( false );
            expectSequence = true;
        }
    }

    for ( auto && unitig : unitigs ) {
        for ( UnitigRead const & unitigRead : unitig.reads ) {
            std::string sequence( sequences[unitigRead.first.first] );
            if ( unitigRead.first.second ) {
                std::reverse( sequence.begin(), sequence.end());
                for ( auto && c: sequence ) { c = comp_tab[c]; }
            }
            unitig.sequence += sequence.substr( 0, (size_t) unitigRead.second );
        }
    }

    TIMER_END(__func__);
}



void logUnitigs(  const Unitigs & unitigs, const ReadTrims & readTrims ) {
#ifdef LOG_UNITIGS
    auto   fp = stdout;
    size_t i( 0 );
    char   name[32];

    for ( Unitig const & unitig : unitigs ) {
        sprintf( name, "utg%.6ld%c", i++ + 1, "lc"[unitig.isCircular] );
        fprintf( fp,
                 "S\t%s\t%s\tLN:i:%d\n",
                 name,
                 !unitig.sequence.empty() && LOG_UNITIGS_SEQUENCES ? unitig.sequence.c_str() : "*",
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
        printf("%d\n",unitig.length);
    }
#endif
}
