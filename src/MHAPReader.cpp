//
// Created by Domagoj Boros on 04/11/2016.
//

#include <iostream>
#include <fstream>
#include "Overlap.h"

#include "MHAPReader.h"

int MHAPReader::read(const std::string &path, std::vector<Overlap>& overlaps) {

    std::ifstream is;
    is.open(path);

    if (!is.is_open()) {
        std::cerr << "Unable to open file " << path << std::endl;
        return 0;
    }

    std::string line;
    int aId, bId;
    float err;
    int minmers;
    int arc, as, ae, al;
    int brc, bs, be, bl;

    int numoverlaps = 0;
    while (is >> aId >> bId >> err >> minmers >> arc >> as >> ae >> al >> brc >> bs >> be >> bl) {
        overlaps.emplace_back(aId, bId, !arc, as, ae, al, !brc, bs, be, bl);
        numoverlaps++;
    }

    return numoverlaps;
}
