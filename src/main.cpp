#include <iostream>

#include "common.h"

#include "IO.h"
#include "OverlapUtils.h"


void trimReads(Overlaps &overlaps, const ReadTrims &readTrims, Params params) {
    Overlaps newOverlaps;

    for (const auto &overlap : overlaps) {
        auto const trimA(readTrims.at(overlap.aId()));
        auto const trimB(readTrims.at(overlap.bId()));

        // read A or read B is considered invalid so delete overlap between them
        if (trimA.del || trimB.del) continue;

        read_size_t aStartNew, aEndNew, bStartNew, bEndNew;

        if (overlap.isReversed()) {
            if (overlap.bEnd() < trimB.end) {
                aStartNew = overlap.aStart();
            } else {
                aStartNew = overlap.aStart() + (overlap.bEnd() - trimB.end);
            }
            if (overlap.bStart() > trimB.start) {
                aEndNew = overlap.aEnd();
            } else {
                aEndNew = overlap.aEnd() - (trimB.start - overlap.bStart());
            }
            if (overlap.aEnd() < trimA.end) {
                bStartNew = overlap.bStart();
            } else {
                bStartNew = overlap.bStart() + (overlap.aEnd() - trimA.end);
            }
            if (overlap.aStart() > trimA.start) {
                bEndNew = overlap.bEnd();
            } else {
                bEndNew = overlap.bEnd() - (trimA.start - overlap.aStart());
            }
        } else {
            if (overlap.bStart() > trimB.start) {
                aStartNew = overlap.aStart();
            } else {
                aStartNew = overlap.aStart() + (trimB.start - overlap.bStart());
            }
            if (overlap.bEnd() < trimB.end) {
                aEndNew = overlap.aEnd();
            } else {
                aEndNew = overlap.aEnd() - (overlap.bEnd() - trimB.end);
            }
            if (overlap.aStart() > trimA.start) {
                bStartNew = overlap.bStart();
            } else {
                bStartNew = overlap.bStart() + (trimA.start - overlap.aStart());
            }
            if (overlap.aEnd() < trimA.end) {
                bEndNew = overlap.bEnd();
            } else {
                bEndNew = overlap.bEnd() - (overlap.aEnd() - trimA.end);
            }
        }
        if (aStartNew > trimA.start) {
            aStartNew = aStartNew - trimA.start;
        } else {
            aStartNew = trimA.start - trimA.start;
        }
        if (aEndNew < trimA.end) {
            aEndNew = aEndNew - trimA.start;
        } else {
            aEndNew = trimA.end - trimA.start;
        }
        if (bStartNew > trimB.start) {
            bStartNew = bStartNew - trimB.start;
        } else {
            bStartNew = trimB.start - trimB.start;
        }
        if (bEndNew < trimB.end) {
            bEndNew = bEndNew - trimB.start;
        } else {
            bEndNew = trimB.end - trimB.start;
        }

        auto const aSpanNew(aEndNew - aStartNew);
        auto const bSpanNew(bEndNew - bStartNew);
        if (aSpanNew < params.minAllowedMatchSpan || bSpanNew < params.minAllowedMatchSpan) continue;

        double r = (double) (aSpanNew + bSpanNew) / (overlap.aSpan() + overlap.bSpan());

        auto const alignmentBlockLength(static_cast<int>(overlap.alignmentBlockLength() * r + 0.499));
        auto const numberOfSequenceMatches(static_cast<int>(overlap.numberOfSequenceMatches() * r + 0.499));

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

    }
    overlaps.swap(newOverlaps);

    std::cout << "Remained " << overlaps.size() << " overlaps" << std::endl;
}


int main() {

    Overlaps overlaps;
    Reads reads;
    Params params(getDefaultParams());

    std::cout << "1) Reading overlaps and reads" << std::endl;
    loadPAF(overlaps, reads, "../test-data/lambda_overlaps.paf", params);

    std::cout << "2) Proposing read trims" << std::endl;
    ReadTrims readTrims;
    proposeReadTrims(readTrims, overlaps, params, false);

    std::cout << "3) Trimming reads" << std::endl;
    trimReads(overlaps, readTrims, params);


//    std::cout << "Left with " << overlaps.size() << " trimmed overlaps" << std::endl;

//
//    for (auto const &readTrim: readTrims) {
//        std::cout << readTrim.first << " " << readTrim.second.toString() << std::endl;
//    }



//    writeOverlapsToSIF("mnebijemte.sif", overlaps);


    return 0;
}
