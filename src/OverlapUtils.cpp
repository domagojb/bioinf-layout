//
// Created by Domagoj Boros on 04/12/2016.
//

#include "OverlapUtils.h"


#define MATCH_BEGIN 0
#define MATCH_END (!(MATCH_BEGIN))

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
            points.emplace_back(aStartNew, MATCH_BEGIN);
            points.emplace_back(aEndNew, MATCH_END);
        }
    }

    std::sort(points.begin(), points.end());
}


void proposeReadTrims(
        ReadTrims &readTrims,
        read_size_t minimalReadCoverage,
        float minimalIdentityFactor,
        read_size_t endClipping,
        const Overlaps &overlaps
)
{
    // isolating overlaps by their query(aId) reads and proposing cuts and deletions based on them
    // overlaps are assumed to be sorted based on their query(aId) read
    // outer for loop iterates over all overlaps and when encounters a diff between last and new query(aId) read
    // it processes all overlaps between two diff occurrences,
    // therefore variable <last> remembers last time the diff has occurred


    auto last(0UL);
    auto overlapsCount(overlaps.size());
    for (auto i(1UL); i <= overlapsCount; ++i) {
        int aId(overlaps[i - 1].aId());

        if (i == overlapsCount || aId != overlaps[i].aId()) {
            // all overlaps with same aId are overlaps[last:i-1]std::vector<std::pair<int, bool>> matchesPointsCollectorVector;

            std::vector<std::pair<int, bool>> points;

            // extract start and end points at the query for all overlaps on it
            extractPoints(points, minimalIdentityFactor, endClipping, last, i, aId, overlaps);


            // do some magic to extract the largest portion of read which is covered with at least minimalReadCoverage
            // overlaps, similar to finding part of math expression where there are at least 3 brackets around:
            // (((()()))())
            //    |--|      <- find this part

            ReadTrim max, max2;
            int begin(0);
            decltype(minimalReadCoverage) readCoverage = 0;
            for (auto const &point: points) {
                auto const oldReadCoverage(readCoverage);

                auto const pointPosition(point.first);
                auto const pointType(point.second);


                pointType == MATCH_BEGIN ? ++readCoverage : --readCoverage;

                if (oldReadCoverage < minimalReadCoverage && minimalReadCoverage <= readCoverage) {
                    begin = pointPosition;
                } else if (readCoverage < minimalReadCoverage && minimalReadCoverage <= oldReadCoverage) {
                    auto len(pointPosition - begin);
                    if (len > max.end - max.begin) {
                        max2 = max;
                        max.begin = begin;
                        max.end = pointPosition;
                    } else if (len > max2.end - max2.begin) {
                        max2.begin = begin;
                        max2.end = pointPosition;
                    }
                }
            }

            // add trim if it is sane
            if(max.begin < max.end) readTrims[aId] = ReadTrim(max.begin - endClipping, max.end + endClipping, true);

            last = i;
        }
    }

}

#undef IS_END
#undef IS_START