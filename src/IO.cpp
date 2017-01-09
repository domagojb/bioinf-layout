//
// Created by Domagoj Boros on 04/11/2016.
//

#include <iostream>
#include <fstream>
#include <algorithm>

#include "IO.h"

void writeOverlapsToSIF( const std::string & path, const Overlaps & overlaps ) {

    std::ofstream os;
    os.open( path );

    char edgeType[2] = { 0, 0 };
    for ( const auto & overlap : overlaps ) {
        os << overlap.aId() << " " << "stipe" << " " << overlap.bId() << std::endl;
        //        edgeType[0] = (overlap->isBrc()) ? 'a' : 'b';
        //        os << overlap->getAId() << " " << edgeType << " " << overlap->getBId() << std::endl;
    }

    os.flush();
    os.close();
}

void loadPAF( Overlaps & overlaps, const std::string & path, const Params & params ) {
    std::ifstream is;
    is.open( path );

    if ( !is.is_open()) {
        std::cerr << "Unable to open file " << path << std::endl;
        return;
    }

    std::string line;
    read_id_t     aId, bId;
    read_size_t aStart, aEnd, aLength;
    read_size_t bStart, bEnd, bLength;
    char          relativeStrand; // - or +

    read_size_t numberOfSequenceMatches; // the number of sequence matches
    read_size_t alignmentBlockLength; // the total number of sequence matches, mismatches, insertions and deletions in the alignment

    std::string placeholder1, placeholder2;

    while ( is
            >> aId
            >> aLength
            >> aStart
            >> aEnd
            >> relativeStrand
            >> bId
            >> bLength
            >> bStart
            >> bEnd
            >> numberOfSequenceMatches
            >> alignmentBlockLength >> placeholder1 >> placeholder2 ) {

        read_size_t aMatchSpan( aEnd - aStart );
        read_size_t bMatchSpan( bEnd - bStart );

        if ( aMatchSpan < params.minAllowedMatchSpan ) continue;
        if ( bMatchSpan < params.minAllowedMatchSpan ) continue;
        if ( numberOfSequenceMatches < params.minAllowedNumberOfSequenceMatches ) continue;


        bool isReversed( relativeStrand == '-' );

        // add a->b
        overlaps.emplace_back( aId,
                               aLength,
                               aStart,
                               aEnd,
                               isReversed,
                               bId,
                               bLength,
                               bStart,
                               bEnd,
                               numberOfSequenceMatches,
                               alignmentBlockLength
                             );

        if ( aId != bId ) {

            // add b->a
            overlaps.emplace_back( bId,
                                   bLength,
                                   bStart,
                                   bEnd,
                                   isReversed,
                                   aId,
                                   aLength,
                                   aStart,
                                   aEnd,
                                   numberOfSequenceMatches,
                                   alignmentBlockLength
                                 );

        }
    }

    // sort overlaps

    std::sort( overlaps.begin(), overlaps.end());

    std::cout << "Read " << overlaps.size() << " overlaps" << std::endl;
}

void logOverlaps( const Overlaps & overlaps ) {

    fprintf( stdout, "aId\taLen\taS\taE\tor\tbLen\tbS\tbE\tml\tbl\n" );

    for ( auto const & overlap:overlaps ) {
        fprintf( stdout, "%s\n", overlap.toString().c_str());
    }
}

void logTrimmedOverlap( const Overlap & overlap, const ReadTrims & readTrims ) {

    const ReadTrim & aTrim( readTrims.at( overlap.aId()));
    const ReadTrim & bTrim( readTrims.at( overlap.bId()));

    fprintf( stdout,
             "%05d:%05d-%05d\t%5d\t%5d\t%5d\t%c\t%05d:%05d-%05d\t%5d\t%5d\t%5d\t%5d\t%5d\t255\n",
             overlap.aId(),
             aTrim.start + 1,
             aTrim.end,
             aTrim.length(),
             overlap.aStart(),
             overlap.aEnd(),
             "+-"[overlap.isReversed()],
             overlap.bId(),
             bTrim.start + 1,
             bTrim.end,
             bTrim.length(),
             overlap.bStart(),
             overlap.bEnd(),
             overlap.numberOfSequenceMatches(),
             overlap.alignmentBlockLength());
}

void logTrimmedOverlaps( const Overlaps & overlaps, const ReadTrims & readTrims ) {

    fprintf( stdout, "aId\taLen\taS\taE\tor\tbLen\tbS\tbE\tml\tbl\n" );

    for ( auto const & overlap:overlaps ) {
        logTrimmedOverlap( overlap, readTrims );
    }
}

void writeGraphToSIF( const std::string & path, const Graph & graph ) {
    std::ofstream os;
    os.open( path );

    for ( const auto & p : graph ) {
        for ( const auto & e : p.second ) {
            os << e.aId << "ab"[e.aIsReversed] << " s " << e.bId << "ab"[e.bIsReversed] << std::endl;
        }
        //        os << overlap->getAId() << " " << edgeType << " " << overlap->getBId() << std::endl;
    }

    os.flush();
    os.close();
}
