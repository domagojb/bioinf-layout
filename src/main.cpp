#include <iostream>

#include "common.h"

#include "IO.h"



int main() {

    Overlaps overlaps;
    Reads reads;
    Params params(getDefaultParams());

    loadPAF(overlaps,reads,"../test-data/lambda_overlaps.paf", params);
    std::cout << "Read " << overlaps.size() << " overlaps" << std::endl;
    std::cout << "Read " << reads.size() << " reads" << std::endl;

    return 0;
}
