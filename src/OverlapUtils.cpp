//
// Created by Domagoj Boros on 04/12/2016.
//

#include "Overlap.h"

#include "OverlapUtils.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))


void filterInternalMatches(const std::vector<Overlap *>& source, std::vector<Overlap *>& dest, int o, float r) {

    for (const auto overlap : source) {
        int overhang = MIN(overlap->getAStart(), overlap->getBStart()) - MIN(overlap->getALength() - overlap->getAEnd(), overlap->getBLength() - overlap->getBEnd());
        int maplen = MAX(overlap->getAEnd() - overlap->getAStart(), overlap->getBEnd() - overlap->getBStart());
        if (overhang > MIN(o, maplen * r)) continue;
        dest.push_back(overlap);
    }
}

void filterContained(const std::vector<Overlap *>& contained, std::vector<Overlap *>& noncontained) {

    for (const auto overlap : contained) {
        if (overlap->getAStart() <= overlap->getBStart() && overlap->getALength() - overlap->getAEnd() <= overlap->getBLength() - overlap->getBEnd()) continue;
        if (overlap->getAStart() >= overlap->getBStart() && overlap->getALength() - overlap->getAEnd() >= overlap->getBLength() - overlap->getBEnd()) continue;
        noncontained.push_back(overlap);
    }
}