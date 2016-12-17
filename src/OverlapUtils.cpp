//
// Created by Domagoj Boros on 04/12/2016.
//

#include "OverlapUtils.h"
#include "params.h"

#define MATCH_START 0
#define MATCH_END (!(MATCH_START))

static bool isChimeric(size_t start, size_t end, Overlaps& overlaps, ReadTrims& readTrims, Params& params) {

    std::vector<std::pair<int, bool>> lefties; // checks left overhang
    std::vector<std::pair<int, bool>> righties; // checks right overhand
    for (size_t i = start; i < end; i++) {
        const Overlap& o = overlaps[i];
        int alen = readTrims[o.aId()].end - readTrims[o.aId()].start;
        int blen = readTrims[o.bId()].end - readTrims[o.bId()].start;

        int alhang = o.aStart();
        int arhang = alen - o.aEnd();

        int blhang = o.isReversed() ? blen - o.bEnd() : o.bStart();
        int brhang = o.isReversed() ? o.bStart() : blen - o.bEnd();

        if (alhang < params.maxOverhang && alhang < blhang) {
            if (arhang > params.maxOverhang && brhang > params.maxOverhang) {
                lefties.emplace_back(arhang, true);
            } else if (arhang > brhang && brhang < params.maxOverhang) {
                lefties.emplace_back(arhang, false);
            }
        } else if (arhang < params.maxOverhang && arhang < brhang) {
            if (alhang > params.maxOverhang && brhang > params.maxOverhang) {
                righties.emplace_back(alhang, true);
            } else if (alhang > blhang && blhang < params.maxOverhang) {
                righties.emplace_back(alhang, false);
            }
        }
    }

    if (lefties.size() < params.minimalReadCoverage) return false;
    if (righties.size() < params.minimalReadCoverage) return false;

    std::sort(lefties.begin(), lefties.end());

    std::sort(rightie.begin(), righties.end());

    int max;
    int ac = 0;
    int bc = 0;
    for (const auto& pair : lefties) {
        if (pair.second) ac++;
        else bc++;
        max = max > ac - bc ? mac : ac - bc;
    }

    if (max >= params.minimalReadCoverage) return true;

    max = 0;
    ac = 0;
    bc = 0;
    for (const auto& pair : righties) {
        if (pair.second) ac++;
        else bc++;
        max = max > ac - bc ? mac : ac - bc;
    }

    if (max >= params.minimalReadCoverage) return true;
}

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

void filterChimeric(Overlaps& overlaps, ReadTrims& readTrims, Params& params) {

    size_t start = 0;

    for (size_t i = 1; i <= overlaps.size(); i++) {
        if (i == overlaps.size() || overlaps[i].aId() != overlaps[start].aId()) {
            readTrims[overlaps[i].aId()].del = isChimeric(start, i, overlaps, readTrims, params);
            start = i;
        }
    }
}

#undef IS_END
#undef IS_START