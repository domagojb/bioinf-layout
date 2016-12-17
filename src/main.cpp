#include <iostream>

#include "common.h"

#include "IO.h"
#include "OverlapUtils.h"

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

    filterReads(overlaps, readTrims, params);

    logTrimmedOverlaps(overlaps, readTrims);

//    std::cout << "Left with " << overlaps.size() << " trimmed overlaps" << std::endl;

//
//    for (auto const &readTrim: readTrims) {
//        std::cout << readTrim.first << " " << readTrim.second.toString() << std::endl;
//    }



//    writeOverlapsToSIF("mnebijemte.sif", overlaps);


    return 0;
}
