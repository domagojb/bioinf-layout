#include <iostream>
#include <vector>

#include "Read.h"
#include "Overlap.h"
#include "IO.h"
#include "OverlapUtils.h"

int main() {
//    std::vector<Read *> reads;

//    std::cout << "Read " << readFASTAReads("../test-data/lambda_reads.fasta", reads) << " reads" << std::endl;

    std::vector<Overlap *> overlaps;
    std::cout << "Read " << readMHAPOverlaps("../test-data/lambda_overlaps.mhap", overlaps) << " overlaps" << std::endl;

    std::vector<Overlap *> nonInternalMatches;
    filterInteralMathches(overlaps, nonInternalMatches);
    std::cout << "Filtered internal matches: " << nonInternalMatches.size() << " remain" << std::endl;

    std::vector<Overlap *> nonContained;
    filterContained(nonInternalMatches, nonContained);
    std::cout << "Filtered contained overlaps: " << nonContained.size() << " remaining" << std::endl;

    writeOverlapsToSIF("../test-data/lambda_graph_noncontained.sif", nonContained);

    return 0;
}
