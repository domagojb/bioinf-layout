#include <iostream>
#include <vector>

#include "Overlap.h"
#include "MHAPReader.h"

int main() {

    std::vector<Overlap> overlaps;
    std::cout << "Read " << MHAPReader::read("../test-data/ecoli_overlap.mhap", overlaps) << " overlaps" << std::endl;

    return 0;
}