//
// Created by Domagoj Boros on 04/12/2016.
//

#include <cmath>
#include <algorithm>
#include <iostream>
#include "OverlapUtils.h"

static bool
isChimeric( size_t start, size_t end, const Overlaps & overlaps, ReadTrims & readTrims, const Params & params ) {
    std::vector<std::pair<int, bool>> lefties; // checks left overhang
    std::vector<std::pair<int, bool>> righties; // checks right overhand
    for ( size_t                      i (start) ; i < end; i++ ) {
        const Overlap & o = overlaps[i];

        int alen   = readTrims[ o.aId()].length();
        int blen   = readTrims[ o.bId()].length();

        int alhang = o.aStart();
        int arhang = alen - o.aEnd();

        int blhang = o.isReversed() ? blen - o.bEnd() : o.bStart();
        int brhang = o.isReversed() ? o.bStart() : blen - o.bEnd();

        if ( alhang < params.maximalOverhangLength && alhang < blhang ) {
            if ( arhang > params.maximalOverhangLength && brhang > params.maximalOverhangLength ) {
                lefties.emplace_back( arhang, true );
            } else if ( arhang > brhang && brhang < params.maximalOverhangLength ) {
                lefties.emplace_back( arhang, false );
            }
        } else if ( arhang < params.maximalOverhangLength && arhang < brhang ) {
            if ( alhang > params.maximalOverhangLength && blhang > params.maximalOverhangLength ) {
                righties.emplace_back( alhang, true );
            } else if ( alhang > blhang && blhang < params.maximalOverhangLength ) {
                righties.emplace_back( alhang, false );
            }
        }
    }
    if ( lefties.size() < params.minimalReadCoverage ) return false;
    if ( righties.size() < params.minimalReadCoverage ) return false;

    std::sort( lefties.begin(), lefties.end());

    std::sort( righties.begin(), righties.end());


    int max = 0;
    int ac  = 0;
    int bc  = 0;
    for ( const auto & pair : lefties ) {
        pair.second ? ac++ : bc++;
        max = max > ac - bc ? max : ac - bc;
    }

    if ( max >= params.minimalReadCoverage ) return true;

    max = 0;
    ac  = 0;
    bc  = 0;
    for ( const auto & pair : righties ) {
        pair.second ? ac++ : bc++;
        max = max > ac - bc ? max : ac - bc;
    }

    return max >= params.minimalReadCoverage;
}

static void extractPoints( std::vector<std::pair<read_id_t , bool>> & points,
                    const Overlaps & overlaps,
                    size_t beginIdx,
                    size_t endIdx,
                    int aId,
                   const Params & params
) {
    for ( auto i( beginIdx ); i < endIdx; ++i ) {

        auto const & overlap( overlaps[i] );

        // if overlap is a match with itself, ignore it
        if ( aId == overlap.bId() ) continue;

        // large relative levenshtein distance of a match?
        // todo: not in paper, could be deleted
        if ( overlap.numberOfSequenceMatches() < overlap.alignmentBlockLength() * params.minimalIdentityFactor ) continue;

        // only accept overlaps with match span at least 2 * endClipping == minAllowedMatchSpan
        auto const aStartNew( overlap.aStart()  );
        auto const aEndNew( overlap.aEnd()  );

        if ( aStartNew + params.minAllowedMatchSpan < aEndNew ) {
            points.emplace_back( aStartNew, false );
            points.emplace_back( aEndNew, true);
        }
    }

    std::sort( points.begin(), points.end());
}


void proposeReadTrims( ReadTrims & readTrims, const Overlaps & overlaps, const Params & params ) {
    TIMER_START("Proposing read trims...");
    // isolating overlaps by their query(aId) reads and proposing cuts and deletions based on them
    // overlaps are assumed to be sorted based on their query(aId) read
    // outer for loop iterates over all overlaps and when it encounters a diff between last and new query(aId) read
    // it processes all overlaps between two diff occurrences (all overlaps with same query(aId)_,
    // therefore variable <last> remembers last time the diff has occurred

    std::vector<std::pair<read_size_t, bool>> points;

//    read_size_t endClipping( params.minAllowedMatchSpan / 2 );
    auto        last( 0UL );
    auto        overlapsCount( overlaps.size());
    auto        saneTrimCounter( 0UL );
    for ( auto  i( 1UL ); i <= overlapsCount; ++i ) {
        int aId( overlaps[i - 1].aId());

        if ( i == overlapsCount || aId != overlaps[i].aId()) {
            points.clear();

            // all overlaps with same aId are overlaps[last:i-1]
            // extract start and end points at the query for all overlaps on it
            extractPoints( points, overlaps, last, i, aId, params );

            // do some magic to extract the largest portion of read which is covered with at least minimalReadCoverage
            // overlaps, similar to finding part of math expression where there are at least 3 brackets around:
            // (((()()))())
            //    |--|      <- find this part

            read_size_t best_start = 0, best_end = 0;
            read_size_t start( 0 );
            int         readCoverage = 0;
            for ( auto const & point: points ) {
                auto const oldReadCoverage( readCoverage );

                read_size_t pointPosition( point.first );
                bool        pointIsEnd( point.second );

                pointIsEnd ? --readCoverage : ++readCoverage;

                if ( oldReadCoverage < params.minimalReadCoverage && params.minimalReadCoverage == readCoverage ) {
                    // entering candidate with minimalReadCoverage overlaps
                    start = pointPosition;
                } else if ( readCoverage < params.minimalReadCoverage &&
                            params.minimalReadCoverage == oldReadCoverage ) {
                    // leaving candidate with minimalReadCoverage overlaps
                    read_size_t len( pointPosition - start );
                    if ( len > best_end - best_start ) {
                        // if candidate is currently the largest region with minimalReadCoverage overlaps remember it
                        best_start = start;
                        best_end   = pointPosition;
                    }
                }
            }

            // if it hasn't found any good region mark the read for deletion
            bool isSaneTrim = best_start + params.minAllowedMatchSpan < best_end;
            readTrims[aId] = ReadTrim( best_start, best_end, !isSaneTrim );
            if ( isSaneTrim ) ++saneTrimCounter;

            last = i;
        }
    }
    std::cout << "Remained " << saneTrimCounter << " sane trims" << std::endl;
    TIMER_END("Done proposing, time passed: ");

}


void trimReads( Overlaps & overlaps, ReadTrims & readTrims, const Params params ) {
    TIMER_START("Trimming reads...");

    // trimming proposed reads

    Overlaps newOverlaps;

    newOverlaps.reserve(overlaps.size());

    for ( const auto & overlap : overlaps ) {
        auto const & aTrim( readTrims[ overlap.aId()]);
        auto const &bTrim( readTrims[overlap.bId()]);

        // if read A or read B is deleted, delete overlap between them (do not emplace it to new overlap vec)
        if ( aTrim.del || bTrim.del ) continue;

        read_size_t aStartNew, aEndNew, bStartNew, bEndNew;

        // trim overlaps based on read trims, if there is overlap that exceeds read trim region, trim the read also
        if ( overlap.isReversed()) {
            if ( overlap.bEnd() < bTrim.end ) {
                aStartNew = overlap.aStart();
            } else {
                aStartNew = overlap.aStart() + ( overlap.bEnd() - bTrim.end );
            }
            if ( overlap.bStart() > bTrim.start ) {
                aEndNew = overlap.aEnd();
            } else {
                aEndNew = overlap.aEnd() - ( bTrim.start - overlap.bStart());
            }
            if ( overlap.aEnd() < aTrim.end ) {
                bStartNew = overlap.bStart();
            } else {
                bStartNew = overlap.bStart() + ( overlap.aEnd() - aTrim.end );
            }
            if ( overlap.aStart() > aTrim.start ) {
                bEndNew = overlap.bEnd();
            } else {
                bEndNew = overlap.bEnd() - ( aTrim.start - overlap.aStart());
            }
        } else {
            if ( overlap.bStart() > bTrim.start ) {
                aStartNew = overlap.aStart();
            } else {
                aStartNew = overlap.aStart() + ( bTrim.start - overlap.bStart());
            }
            if ( overlap.bEnd() < bTrim.end ) {
                aEndNew = overlap.aEnd();
            } else {
                aEndNew = overlap.aEnd() - ( overlap.bEnd() - bTrim.end );
            }
            if ( overlap.aStart() > aTrim.start ) {
                bStartNew = overlap.bStart();
            } else {
                bStartNew = overlap.bStart() + ( aTrim.start - overlap.aStart());
            }
            if ( overlap.aEnd() < aTrim.end ) {
                bEndNew = overlap.bEnd();
            } else {
                bEndNew = overlap.bEnd() - ( overlap.aEnd() - aTrim.end );
            }
        }

        // convert overlap to read trim coordinate system
        if ( aStartNew > aTrim.start ) {
            aStartNew = aStartNew - aTrim.start;
        } else {
            aStartNew = 0;
        }
        if ( aEndNew < aTrim.end ) {
            aEndNew = aEndNew - aTrim.start;
        } else {
            aEndNew = aTrim.end - aTrim.start;
        }
        if ( bStartNew > bTrim.start ) {
            bStartNew = bStartNew - bTrim.start;
        } else {
            bStartNew = 0;
        }
        if ( bEndNew < bTrim.end ) {
            bEndNew = bEndNew - bTrim.start;
        } else {
            bEndNew = bTrim.end - bTrim.start;
        }

        read_size_t aSpanNew( aEndNew - aStartNew );
        read_size_t bSpanNew( bEndNew - bStartNew );

        // if match span is too small discard overlap
        if ( aSpanNew < params.minAllowedMatchSpan || bSpanNew < params.minAllowedMatchSpan ) continue;


        // brother to brother
        double r = (double) ( aSpanNew + bSpanNew ) / ( overlap.aSpan() + overlap.bSpan());
        read_size_t alignmentBlockLength( static_cast<read_size_t>(std::round( overlap.alignmentBlockLength() * r )));
        read_size_t numberOfSequenceMatches( static_cast<read_size_t>(std::round( overlap.numberOfSequenceMatches() * r
                                                                                )));

        // construct new possibly trimmed and shifted overlap
        newOverlaps.emplace_back( overlap.aId(),
                                  overlap.aLength(),
                                  aStartNew,
                                  aEndNew,
                                  overlap.isReversed(),
                                  overlap.bId(),
                                  overlap.bLength(),
                                  bStartNew,
                                  bEndNew,
                                  numberOfSequenceMatches,
                                  alignmentBlockLength
                                );
    }
    overlaps.swap( newOverlaps );

    std::cout << "Remained " << overlaps.size() << " overlaps" << std::endl;
    TIMER_END("Done trimming, time passed: ");
}

void filterInternalReads( Overlaps & overlaps, ReadTrims & readTrims, const Params params ) {
    TIMER_START("Filtering internal reads...");

    Overlaps newOverlaps;

    newOverlaps.reserve(overlaps.size());

    OverlapClassification overlapClassification;
    Edge                  edge;

    for ( const auto & overlap : overlaps ) {
        classifyOverlapAndMeasureItsLength( overlapClassification,
                                            edge,
                                            overlap,
                                            readTrims[overlap.aId()].length(),
                                            readTrims[ overlap.bId()].length(),
                                            params.maximalOverhangLength * 1.5,
                                            0.5//params.mappingLengthRatio,
                                          );
        if(overlapClassification == OVERLAP_INTERNAL_MATCH) continue;

        newOverlaps.emplace_back( overlap );
    }

    overlaps.swap( newOverlaps );


    fprintf( stdout, "%ld hits remain after filtering internal reads\n", overlaps.size() );

    TIMER_END("Done filtering, time passed: ");
}

void classifyOverlapAndMeasureItsLength( OverlapClassification & overlapClassification,
                                         Edge & edge,
                                         const Overlap overlap,
                                         read_size_t aLength,
                                         read_size_t bLength,
                                         read_size_t maximalOverhangLength,
                                         float mappingLengthRatio ) {
    read_size_t bLengthLeft, bLengthRight, overhangLeft, overhangRight;
    read_size_t aStart( overlap.aStart());

    // daj mi manje overhenge
    if ( overlap.isReversed()) {
        bLengthLeft  = bLength - overlap.bEnd();
        bLengthRight = overlap.bStart();
    } else {
        bLengthLeft  = overlap.bStart();
        bLengthRight = bLength - overlap.bEnd();
    }
    if ( aStart < bLengthLeft ) {
        overhangLeft = aStart;
    } else {
        overhangLeft = bLengthLeft;
    }
    if ( aLength - overlap.aEnd() < bLengthRight ) {
        overhangRight = aLength - overlap.aEnd();
    } else {
        overhangRight = bLengthRight;
    }


    if ( overhangLeft > maximalOverhangLength || overhangRight > maximalOverhangLength ) {
        overlapClassification = OVERLAP_INTERNAL_MATCH;
        return;
    }

    if ( overlap.aEnd() - aStart < ( overlap.aEnd() - aStart + overhangLeft + overhangRight ) * mappingLengthRatio ) {
        overlapClassification = OVERLAP_INTERNAL_MATCH;
        return;
    }

    if ( aStart <= bLengthLeft && aLength - overlap.aEnd() <= bLengthRight ) {
        overlapClassification = OVERLAP_A_CONTAINED;
        return;
    } else if ( aStart >= bLengthLeft && aLength - overlap.aEnd() >= bLengthRight ) {
        overlapClassification = OVERLAP_B_CONTAINED;
        return;
    } else if ( aStart > bLengthLeft ) {
        edge.aIsReversed   = false;
        edge.bIsReversed   = overlap.isReversed();
        edge.overlapLength = aStart - bLengthLeft;
    } else {
        edge.aIsReversed   = true;
        edge.bIsReversed   = !overlap.isReversed();
        edge.overlapLength = ( aLength - overlap.aEnd()) - bLengthRight;
    }
    overlapClassification = OVERLAP_A_TO_B_OR_B_TO_A;

    edge.aId = overlap.aId();
    edge.bId = overlap.bId();

    edge.del                          = 0;
    edge.numberOfSequenceMatches      = overlap.numberOfSequenceMatches();
    edge.numberOfSequenceMatchesRatio = static_cast<float>(overlap.numberOfSequenceMatches()) /
                                        overlap.alignmentBlockLength();
}

void filterChimeric( const Overlaps & overlaps, ReadTrims & readTrims, const Params & params ) {
    TIMER_START("Filtering chimeric...");

    size_t chimericCnter = 0;

    for ( size_t i(1), start(0); i <= overlaps.size(); i++ ) {
        if ( i == overlaps.size() || overlaps[i].aId() != overlaps[start].aId()) {
            if ( isChimeric( start, i, overlaps, readTrims, params ) ) {
                readTrims[overlaps[start].aId()].del = true;
                ++chimericCnter;
            }
            start = i;
        }
    }

    std::cout << "Found " << chimericCnter << " chimeric reads" << std::endl;
    TIMER_END("Done with chimeric, time passed: ");
}

void filterContained( Overlaps & overlaps, ReadTrims & readTrims, const Params & params ) {
    TIMER_START("Filtering contained...");

    for ( auto & overlap : overlaps ) {
        OverlapClassification overlapClassification;
        Edge                  edge;
        classifyOverlapAndMeasureItsLength( overlapClassification,
                                            edge,
                                            overlap,
                                            readTrims[overlap.aId()].length(),
                                            readTrims[overlap.bId()].length(),
                                            params.maximalOverhangLength,
                                            params.mappingLengthRatio
                                          );
        if ( overlapClassification == OVERLAP_A_CONTAINED ) {
            readTrims[overlap.aId()].del = true;
        } else if ( overlapClassification == OVERLAP_B_CONTAINED ) {
            readTrims[overlap.bId()].del = true;
        }
    }

    // remove overlaps whith deleted reads (sry but I had to practise stl :P )
    overlaps.erase( std::remove_if( overlaps.begin(), overlaps.end(), [ &readTrims ]( const Overlap & o ) {
                                        return readTrims[o.aId()].del || readTrims[o.bId()].del;
                                    }
                                  ), overlaps.end());

    // if a read has no overlaps, mark it for removal and remove it at the end
    for ( auto && readTrim : readTrims ) readTrim.second.counter = 0;

    for ( auto && overlap : overlaps ) {
        ++readTrims[overlap.aId()].counter;
        ++readTrims[overlap.bId()].counter;
    }
    for ( auto && readTrim : readTrims ) if(readTrim.second.counter == 0) readTrim.second.del = true;

    // apparently there is no better way to filter a dict, pythonic way would be dict = filter(lambda x: x.second.del,dict), but hey, dis iz cpp
    for ( auto iter( readTrims.begin()); iter != readTrims.end(); ) {
        if ( iter->second.del ) {
            iter = readTrims.erase( iter );
        } else {
            ++iter;
        }
    }

    std::cout << "Remained " << overlaps.size() << " overlaps" << std::endl;
    std::cout << "Remained " << readTrims.size() << " reads" << std::endl;
    TIMER_END("Done with contained, time passed: ");
}