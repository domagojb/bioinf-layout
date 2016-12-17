//
// Created by Domagoj Boros on 04/12/2016.
//

#include "OverlapUtils.h"
#include "params.h"


#define MATCH_START 0
#define MATCH_END (!(MATCH_START))

void extractPoints(
        std::vector<std::pair<int, bool>> &points,
        float minimalIdentityFactor,
        int endClipping,
        size_t last,
        size_t i,
        int aId,
        const Overlaps &overlaps
)
{
    for (auto j(last); j < i; ++j) {

        auto const &overlap(overlaps[j]);

        auto const bid(overlap.bId());

        if (aId == bid) continue; // match with same read

// large relative levenshtein distance of a match?
        if (overlap.numberOfSequenceMatches() < overlap.alignmentBlockLength() * minimalIdentityFactor)
            continue;

        auto const aStartNew(overlap.aStart() + endClipping);
        auto const aEndNew(overlap.aEnd() - endClipping);

        if (aStartNew < aEndNew) {
            points.emplace_back(aStartNew, MATCH_START);
            points.emplace_back(aEndNew, MATCH_END);
        }
    }

    std::sort(points.begin(), points.end());
}


void proposeReadTrims(
        ReadTrims &readTrims,
        const Overlaps &overlaps,
        const Params & params,
        bool clipEndings
)
{
    // isolating overlaps by their query(aId) reads and proposing cuts and deletions based on them
    // overlaps are assumed to be sorted based on their query(aId) read
    // outer for loop iterates over all overlaps and when encounters a diff between last and new query(aId) read
    // it processes all overlaps between two diff occurrences,
    // therefore variable <last> remembers last time the diff has occurred


    auto endClipping(clipEndings?params.minAllowedMatchSpan/2:0);
    auto last(0UL);
    auto overlapsCount(overlaps.size());
    auto saneTrimCounter(0UL);
    for (auto i(1UL); i <= overlapsCount; ++i) {
        int aId(overlaps[i - 1].aId());

        if (i == overlapsCount || aId != overlaps[i].aId()) {
            // all overlaps with same aId are overlaps[last:i-1]std::vector<std::pair<int, bool>> matchesPointsCollectorVector;

            std::vector<std::pair<int, bool>> points;

            // extract start and end points at the query for all overlaps on it
            extractPoints(points, params.minimalIdentityFactor, endClipping, last, i, aId, overlaps);


            // do some magic to extract the largest portion of read which is covered with at least minimalReadCoverage
            // overlaps, similar to finding part of math expression where there are at least 3 brackets around:
            // (((()()))())
            //    |--|      <- find this part

            ReadTrim max, max2;
            int start(0);
            decltype(params.minimalReadCoverage) readCoverage = 0;
            for (auto const &point: points) {
                auto const oldReadCoverage(readCoverage);

                auto const pointPosition(point.first);
                auto const pointType(point.second);


                pointType == MATCH_START ? ++readCoverage : --readCoverage;

                if (oldReadCoverage < params.minimalReadCoverage && params.minimalReadCoverage <= readCoverage) {
                    start = pointPosition;
                } else if (readCoverage < params.minimalReadCoverage && params.minimalReadCoverage <= oldReadCoverage) {
                    auto len(pointPosition - start);
                    if (len > max.end - max.start) {
                        max2 = max;
                        max.start = start;
                        max.end = pointPosition;
                    } else if (len > max2.end - max2.start) {
                        max2.start = start;
                        max2.end = pointPosition;
                    }
                }
            }

            // if trim start < trim end consider it sane or set delete flag otherwise
            bool isSaneTrim = max.start < max.end;
            readTrims[aId] = ReadTrim(max.start - endClipping, max.end + endClipping, !isSaneTrim);
            if(isSaneTrim) ++saneTrimCounter;

            last = i;
        }
    }
    std::cout<<"Remained "<<saneTrimCounter<<" sane trims"<<std::endl;
}

#undef IS_END
#undef IS_START