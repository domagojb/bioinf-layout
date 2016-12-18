//
// Created by Domagoj Boros on 04/12/2016.
//

#include <cmath>
#include "OverlapUtils.h"
#include "IO.h"
#include "params.h"


#define MATCH_START false
#define MATCH_END (!(MATCH_START))

static bool isChimeric(size_t start, size_t end, const Overlaps& overlaps, const ReadTrims& readTrims,  const Params& params) {
    std::vector<std::pair<int, bool>> lefties; // checks left overhang
    std::vector<std::pair<int, bool>> righties; // checks right overhand
    for (size_t i = start; i < end; i++) {
        const Overlap& o = overlaps[i];

        int alen = readTrims.at(o.aId()).length();
        int blen = readTrims.at(o.bId()).length();
//        printf("%d %d\n",alen,blen);
        int alhang = o.aStart();
        int arhang = alen - o.aEnd();

        int blhang = o.isReversed() ? blen - o.bEnd() : o.bStart();
        int brhang = o.isReversed() ? o.bStart() : blen - o.bEnd();

//        printf("%d %d %d %d %d %d\n",alen,blen,alhang,arhang,blhang,brhang);


        if (alhang < params.maximalOverhangLength && alhang < blhang) {
            if (arhang > params.maximalOverhangLength && brhang > params.maximalOverhangLength) {
                lefties.emplace_back(arhang, true);
//                printf("arhang true\n");
            } else if (arhang > brhang && brhang < params.maximalOverhangLength) {
                lefties.emplace_back(arhang, false);
//                printf("arhang false\n");
//            } else {
//                printf("skip r\n");
            }
        } else if (arhang < params.maximalOverhangLength && arhang < brhang) {
            if (alhang > params.maximalOverhangLength && blhang > params.maximalOverhangLength) {
//                printf("alhang true\n");
                righties.emplace_back(alhang, true);
            } else if (alhang > blhang && blhang < params.maximalOverhangLength) {
                righties.emplace_back(alhang, false);
//                printf("alhang false\n");
//            } else {
//                printf("skip l\n");
            }
//        } else {
//            printf("skip\n");
        }
    }
    if (lefties.size() < params.minimalReadCoverage) return false;
    if (righties.size() < params.minimalReadCoverage) return false;

    std::sort(lefties.begin(), lefties.end());

    std::sort(righties.begin(), righties.end());




    int max=0;
    int ac = 0;
    int bc = 0;
    for (const auto& pair : lefties) {
//        std::cout<<pair.first<<" "<<pair.second<<std::endl;
        if (pair.second) (ac++);
        else (bc++);
        max = max > ac - bc ? max : ac - bc;
    }

    if (max >= params.minimalReadCoverage) {
        return true;
    }

//    std::cout<<lefties.size()<<std::endl;
//    std::cout<<righties.size()<<std::endl;

    max = 0;
    ac = 0;
    bc = 0;
    for (const auto& pair : righties) {
//        std::cout<<pair.first<<" "<<pair.second<<std::endl;
        if (pair.second) ac++;
        else bc++;
        max = max > ac - bc ? max : ac - bc;
    }

    if (max >= params.minimalReadCoverage) {
        return true;
    }
    return false;
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
        const Params &params,
        bool clipEndings
) {
    // isolating overlaps by their query(aId) reads and proposing cuts and deletions based on them
    // overlaps are assumed to be sorted based on their query(aId) read
    // outer for loop iterates over all overlaps and when encounters a diff between last and new query(aId) read
    // it processes all overlaps between two diff occurrences,
    // therefore variable <last> remembers last time the diff has occurred


    read_size_t endClipping(clipEndings ? params.minAllowedMatchSpan / 2 : 0);
    auto last(0UL);
    auto overlapsCount(overlaps.size());
    auto saneTrimCounter(0UL);
    for (auto i(1UL); i <= overlapsCount; ++i) {
        int aId(overlaps[i - 1].aId());

        if (i == overlapsCount || aId != overlaps[i].aId()) {
            // all overlaps with same aId are overlaps[last:i-1]std::vector<std::pair<int, bool>> matchesPointsCollectorVector;

            std::vector<std::pair<read_size_t, bool>> points;

            // extract start and end points at the query for all overlaps on it
            extractPoints(points, params.minimalIdentityFactor, endClipping, last, i, aId, overlaps);


            // do some magic to extract the largest portion of read which is covered with at least minimalReadCoverage
            // overlaps, similar to finding part of math expression where there are at least 3 brackets around:
            // (((()()))())
            //    |--|      <- find this part

            ReadTrim max;
            read_size_t start(0);
            int readCoverage = 0;
            for (auto const &point: points) {
                auto const oldReadCoverage(readCoverage);

                read_size_t pointPosition(point.first);
                bool pointType(point.second);


                pointType == MATCH_START ? ++readCoverage : --readCoverage;

                if (oldReadCoverage < params.minimalReadCoverage && params.minimalReadCoverage == readCoverage) {
                    // if from 2 to 3
                    start = pointPosition;
                } else if (readCoverage < params.minimalReadCoverage && params.minimalReadCoverage == oldReadCoverage) {
                    // if from 3 to 2
                    read_size_t len(pointPosition - start);
                    if (len > max.end - max.start) {
//                        max2 = max;
                        max.start = start;
                        max.end = pointPosition;
//                    } else if (len > max2.end - max2.start) {
//                        max2.start = start;
//                        max2.end = pointPosition;
                    }
                }
            }

            // if trim start < trim end consider it sane or set delete flag otherwise
            bool isSaneTrim = max.start < max.end;
            readTrims[aId] = ReadTrim(max.start - endClipping, max.end + endClipping, !isSaneTrim);
            if (isSaneTrim) ++saneTrimCounter;

            last = i;
        }
    }
    std::cout << "Remained " << saneTrimCounter << " sane trims" << std::endl;
}


void trimReads(Overlaps &overlaps, const ReadTrims &readTrims, const  Params params) {
    Overlaps newOverlaps;

    for (const auto &overlap : overlaps) {
        auto const aTrim(readTrims.at(overlap.aId()));
        auto const bTrim(readTrims.at(overlap.bId()));

        // read A or read B is considered invalid so delete overlap between them
        if (aTrim.del || bTrim.del) continue;

        read_size_t aStartNew, aEndNew, bStartNew, bEndNew;

        if (overlap.isReversed()) {
            if (overlap.bEnd() < bTrim.end) {
                aStartNew = overlap.aStart();
            } else {
                aStartNew = overlap.aStart() + (overlap.bEnd() - bTrim.end);
            }
            if (overlap.bStart() > bTrim.start) {
                aEndNew = overlap.aEnd();
            } else {
                aEndNew = overlap.aEnd() - (bTrim.start - overlap.bStart());
            }
            if (overlap.aEnd() < aTrim.end) {
                bStartNew = overlap.bStart();
            } else {
                bStartNew = overlap.bStart() + (overlap.aEnd() - aTrim.end);
            }
            if (overlap.aStart() > aTrim.start) {
                bEndNew = overlap.bEnd();
            } else {
                bEndNew = overlap.bEnd() - (aTrim.start - overlap.aStart());
            }
        } else {
            if (overlap.bStart() > bTrim.start) {
                aStartNew = overlap.aStart();
            } else {
                aStartNew = overlap.aStart() + (bTrim.start - overlap.bStart());
            }
            if (overlap.bEnd() < bTrim.end) {
                aEndNew = overlap.aEnd();
            } else {
                aEndNew = overlap.aEnd() - (overlap.bEnd() - bTrim.end);
            }
            if (overlap.aStart() > aTrim.start) {
                bStartNew = overlap.bStart();
            } else {
                bStartNew = overlap.bStart() + (aTrim.start - overlap.aStart());
            }
            if (overlap.aEnd() < aTrim.end) {
                bEndNew = overlap.bEnd();
            } else {
                bEndNew = overlap.bEnd() - (overlap.aEnd() - aTrim.end);
            }
        }
        if (aStartNew > aTrim.start) {
            aStartNew = aStartNew - aTrim.start;
        } else {
            aStartNew = 0;
        }
        if (aEndNew < aTrim.end) {
            aEndNew = aEndNew - aTrim.start;
        } else {
            aEndNew = aTrim.end - aTrim.start;
        }
        if (bStartNew > bTrim.start) {
            bStartNew = bStartNew - bTrim.start;
        } else {
            bStartNew = 0;
        }
        if (bEndNew < bTrim.end) {
            bEndNew = bEndNew - bTrim.start;
        } else {
            bEndNew = bTrim.end - bTrim.start;
        }

//        std::cout<<aStartNew<<" "<<aEndNew<<" "<<bStartNew<<" "<<bEndNew<<std::endl;

        read_size_t aSpanNew(aEndNew - aStartNew);
        read_size_t bSpanNew(bEndNew - bStartNew);

        if (aSpanNew < params.minAllowedMatchSpan || bSpanNew < params.minAllowedMatchSpan) continue;

        double r = (double) (aSpanNew + bSpanNew) / (overlap.aSpan() + overlap.bSpan());

        read_size_t alignmentBlockLength(static_cast<read_size_t>(std::round(overlap.alignmentBlockLength() * r)));
        read_size_t numberOfSequenceMatches(
                static_cast<read_size_t>(std::round(overlap.numberOfSequenceMatches() * r)));

        newOverlaps.emplace_back(
                overlap.aId(),
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
//        fprintf(stdout,"%s\n",aTrim.toString().c_str());
//        fprintf(stdout,"%s\n",bTrim.toString().c_str());

//        fprintf(stdout,"%s\n",newOverlaps.back().toString().c_str());
//        logTrimmedOverlap(newOverlaps.back(),readTrims);

    }
    overlaps.swap(newOverlaps);

    std::cout << "Remained " << overlaps.size() << " overlaps" << std::endl;
}

void filterReads(Overlaps &overlaps, const ReadTrims &readTrims, const  Params params) {
    Overlaps newOverlaps;
    uint64_t tot_dp = 0, tot_len = 0;

    for (const auto &overlap : overlaps) {
        auto const &aTrim(readTrims.at(overlap.aId()));
        auto const &bTrim(readTrims.at(overlap.bId()));

        if (aTrim.del || bTrim.del) continue; // todo: check if necessary, I think not

        OverlapClassification overlapClassification;
        Edge edge;
        classifyOverlapAndMeasureItsLength(
                overlapClassification,
                edge,
                overlap,
                aTrim.length(),
                bTrim.length(),
                params.maximalOverhangLength * 1.5,
                0.5,//params.mappingLengthRatio,
                params.minimalOverlap / 2
        );
        switch (overlapClassification) {
            case OVERLAP_INTERNAL_MATCH:
            case OVERLAP_SHORT:
                break;
            case OVERLAP_A_CONTAINED:
                tot_dp += aTrim.length();
                newOverlaps.emplace_back(overlap);
                break;
            case OVERLAP_B_CONTAINED:
                tot_dp += bTrim.length();
                newOverlaps.emplace_back(overlap);
                break;
            case OVERLAP_A_TO_B:
            case OVERLAP_B_TO_A:
                newOverlaps.emplace_back(overlap);
                tot_dp += edge.overlapLength;
                break;
        }
    }

    overlaps.swap(newOverlaps);

    for (size_t i = 1; i <= overlaps.size(); ++i) {
        read_id_t aIdCur = overlaps[i].aId();
        read_id_t aIdLast = overlaps[i - 1].aId();
        if (i == overlaps.size() || aIdCur != aIdLast) {
            tot_len += readTrims.at(aIdLast).length();
        }
    }

    double cov = (double) tot_dp / tot_len;

//    std::cout << tot_len << std::endl;
//    std::cout << tot_dp << std::endl;
    fprintf(stdout, "%ld hits remain after filtering; crude coverage after filtering: %.2f\n", overlaps.size(), cov);

}

void classifyOverlapAndMeasureItsLength(
        OverlapClassification &overlapClassification,
        Edge &edge, const Overlap overlap,
        read_size_t aLength,
        read_size_t bLength,
        read_size_t maximalOverhangLength,
        float mappingLengthRatio,
        read_size_t minimalOverlap
)
{
    read_size_t bLengthLeft, bLengthRight, overhangLeft, overhangRight;
    read_size_t aStart(overlap.aStart());

    // daj mi manje overhenge
    if (overlap.isReversed()) {
        bLengthLeft = bLength - overlap.bEnd();
        bLengthRight = overlap.bStart();
    } else {
        bLengthLeft = overlap.bStart();
        bLengthRight = bLength - overlap.bEnd();
    }
    if (aStart < bLengthLeft) {
        overhangLeft = aStart;
    } else {
        overhangLeft = bLengthLeft;
    }
    if (aLength - overlap.aEnd() < bLengthRight) {
        overhangRight = aLength - overlap.aEnd();
    } else {
        overhangRight = bLengthRight;
    }


    if (overhangLeft > maximalOverhangLength || overhangRight > maximalOverhangLength) {
        overlapClassification = OVERLAP_INTERNAL_MATCH;
        return;
    }

    if (overlap.aEnd() - aStart < (overlap.aEnd() - aStart + overhangLeft + overhangRight) * mappingLengthRatio) {
        overlapClassification = OVERLAP_INTERNAL_MATCH;
        return;
    }

    if (aStart <= bLengthLeft && aLength - overlap.aEnd() <= bLengthRight) {
        overlapClassification = OVERLAP_A_CONTAINED;
        return;
    } else if (aStart >= bLengthLeft && aLength - overlap.aEnd() >= bLengthRight) {
        overlapClassification = OVERLAP_B_CONTAINED;
        return;
    } else if (aStart > bLengthLeft) {
        edge.aIsReversed = false;
        edge.bIsReversed = overlap.isReversed();
        edge.overlapLength = aStart - bLengthLeft;
    } else {
        edge.aIsReversed = true;
        edge.bIsReversed = !overlap.isReversed();
        edge.overlapLength = (aLength - overlap.aEnd()) - bLengthRight;
    }
    if (overlap.aEnd() - aStart + overhangLeft + overhangRight < minimalOverlap ||
        overlap.bEnd() - overlap.bStart() + overhangLeft + overhangRight < minimalOverlap) {
        overlapClassification = OVERLAP_SHORT;
        return;
    }
    overlapClassification = OVERLAP_A_TO_B; // or BTOA

    edge.aId = overlap.aId();
    edge.bId = overlap.bId();

    edge.del = 0;
    edge.numberOfSequenceMatches = overlap.numberOfSequenceMatches();
    edge.numberOfSequenceMatchesRatio = static_cast<float>(overlap.numberOfSequenceMatches()) / overlap.alignmentBlockLength();
}

void filterChimeric(const Overlaps& overlaps, ReadTrims& readTrims,  const Params& params) {

    size_t start = 0;

    size_t chimericCnter = 0;

    for (size_t i = 1; i <= overlaps.size(); i++) {
        if (i == overlaps.size() || overlaps[i].aId() != overlaps[start].aId()) {
            bool b = isChimeric(start, i, overlaps, readTrims, params);
            if(b){
                ++chimericCnter;
            }
            readTrims[overlaps[i].aId()].del = b;
            start = i;
        }
    }

    std::cout<<"Found "<<chimericCnter<<" chimeric reads"<<std::endl;
}

void filterContained(Overlaps& overlaps, ReadTrims& readTrims, const Params& params) {
    size_t tcont=0,qcont=0;

    for (auto& overlap : overlaps) {
        OverlapClassification overlapClassification;
        Edge edge;
        classifyOverlapAndMeasureItsLength(
                overlapClassification,
                edge,
                overlap,
                readTrims[overlap.aId()].length(),
                readTrims[overlap.bId()].length(),
                params.maximalOverhangLength,
                params.mappingLengthRatio,
                params.minimalOverlap
        );
        if (overlapClassification == OVERLAP_A_CONTAINED) {
            readTrims[overlap.aId()].del = true;
            qcont++;
        }
        else if (overlapClassification == OVERLAP_B_CONTAINED) {
            readTrims[overlap.bId()].del = true;
            tcont++;
        }
    }

    // remove overlaps whith deleted reads (sry but I had to practise stl :P )
    overlaps.erase(std::remove_if(overlaps.begin(),overlaps.end(),[&readTrims](const Overlap & o){ return readTrims[o.aId()].del || readTrims[o.bId()].del;}),overlaps.end());


    // apparently there is no better way to filter a dict, pythonic way would be dict = filter(lambda x: x.second.del,dict), but hey, dis iz cpp
    for(auto iter(readTrims.begin()); iter != readTrims.end(); ) {
        if (iter->second.del) {
            readTrims.erase(iter++);
        } else {
            ++iter;
        }
    }

    std::cout << "Remained " << overlaps.size() << " overlaps" << std::endl;
    std::cout << "Remained " << readTrims.size() << " reads" << std::endl;
}

void mergeTrims(ReadTrims &readTrims, const ReadTrims &readTrims2) {
    for (auto &pair : readTrims) {
        read_id_t id(pair.first);

        ReadTrim & readTrim(pair.second);
        if(readTrims2.find(id)==readTrims2.end()) continue;
        const ReadTrim & readTrim2(readTrims2.at(id));

        readTrim.end = readTrim.start + readTrim2.end;
        readTrim.start += readTrim2.start;

    }
}
