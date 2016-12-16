#include <iostream>

#include "common.h"

#include "IO.h"
#include "OverlapUtils.h"



int main() {

    Overlaps overlaps;
    Reads reads;
    Params params(getDefaultParams());

    loadPAF(overlaps, reads, "../test-data/lambda_overlaps.paf", params);
    std::cout << "Read " << overlaps.size() << " overlaps" << std::endl;
    std::cout << "Read " << reads.size() << " reads" << std::endl;
//    logOverlaps(overlaps);

    ReadTrims readTrims;
    proposeReadTrims(readTrims,params.minimalReadCoverage, params.minimalIdentityFactor, 0, overlaps);

    std::cout << "Proposed " << readTrims.size() << " trims" << std::endl;


    for(auto const & readTrim: readTrims){
        std::cout<<readTrim.first<<" "<<readTrim.second.toString()<<std::endl;
    }



//    writeOverlapsToSIF("mnebijemte.sif", overlaps);


    return 0;
}
