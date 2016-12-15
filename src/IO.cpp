//
// Created by Domagoj Boros on 04/11/2016.
//

#include <iostream>
#include <fstream>

#include "IO.h"

//
//int readMHAPOverlaps(const std::string &path, std::vector<Overlap *> &overlaps, const std::vector<Read *> &reads) {
//
//    std::ifstream is;
//    is.open(path);
//
//    if (!is.is_open()) {
//        std::cerr << "Unable to open file " << path << std::endl;
//        return 0;
//    }
//
//    std::string line;
//    int aId, bId;
//    float err;
//    int minmers;
//    int arc, as, ae, al;
//    int brc, bs, be, bl;
//
//    int numoverlaps = 0;
//    while (is >> aId >> bId >> err >> minmers >> arc >> as >> ae >> al >> brc >> bs >> be >> bl) {
////        overlaps.push_back(new Overlap(reads[aId - 1], reads[bId - 1], aId, bId, !arc, as, ae, al, !brc, bs, be, bl));
////        numoverlaps++;
//    }
//
//    return numoverlaps;
//}
//
//int readFASTAReads(const std::string &path, std::vector<Read *> &reads) {
//
//#define BUFFER_SIZE 4096
//
//    FILE *f = fopen(path.c_str(), "r");
//
//    std::string name;
//    std::string sequence;
//
//    char *buffer = new char[BUFFER_SIZE];
//
//    bool isName = false;
//    bool createRead = false;
//
//    size_t idx = 0;
//
//    while (!feof(f)) {
//
//        size_t readLen = fread(buffer, sizeof(char), BUFFER_SIZE, f);
//
//        for (int i = 0; i < readLen; ++i) {
//
//            if (buffer[i] == '>') {
//
//                if (createRead) {
////                    reads.push_back(new Read(idx++, sequence));
//                }
//
//                name.clear();
//                sequence.clear();
//
//                createRead = true;
//                isName = true;
//
//            } else {
//                switch (buffer[i]) {
//                    case '\r':
//                        break;
//                    case '\n':
//                        isName = false;
//                        break;
//                    default:
//                        isName ? name += buffer[i] : sequence += buffer[i];
//                        break;
//                }
//            }
//        }
//    }
//
////    reads.push_back(new Read(idx, sequence));
//
//    delete[] buffer;
//    fclose(f);
//
//    return (int) reads.size();
//#undef BUFFER_SIZE
//}
//
void writeOverlapsToSIF(const std::string &path, const Overlaps &overlaps) {

    std::ofstream os;
    os.open(path);

    char edgeType[2] = {0, 0};
    for (const auto & overlap : overlaps) {
        os<< overlap.aId_<<" "<<"stipe"<<" "<<overlap.bId_<<std::endl;
//        edgeType[0] = (overlap->isBrc()) ? 'a' : 'b';
//        os << overlap->getAId() << " " << edgeType << " " << overlap->getBId() << std::endl;
    }

    os.flush();
    os.close();
}

void loadPAF(Overlaps &overlaps, Reads &reads, const std::string &path, const Params &params) {
    std::ifstream is;
    is.open(path);

    if (!is.is_open()) {
        std::cerr << "Unable to open file " << path << std::endl;
        return;
    }

    std::string line;
    int aId, bId;
    int aStart, aEnd, aLength;
    int bStart, bEnd, bLength;
    char relativeStrand; // - or +

    int numberOfSequenceMatches; // the number of sequence matches
    int alignmentBlockLength; // the total number of sequence matches, mismatches, insertions and deletions in the alignment

    std::string placeholder1, placeholder2;


    while (is
            >> aId
            >> aLength
            >> aStart
            >> aEnd
            >> relativeStrand
            >> bId
            >> bLength
            >> bStart
            >> bEnd
            >> numberOfSequenceMatches
            >> alignmentBlockLength
            >> placeholder1
            >> placeholder2
            ) {

        int aMatchSpan(aEnd - aStart);
        int bMatchSpan(bEnd - bStart);

        if (aMatchSpan < params.minAllowedMatchSpan) continue;
        if (bMatchSpan < params.minAllowedMatchSpan) continue;
        if (numberOfSequenceMatches < params.minAllowedNumberOfSequenceMatches) continue;


        bool isReversed(relativeStrand == '+');

        // add a->b
        overlaps.emplace_back(
                aId,
                aLength,
                aStart,
                aEnd,
                isReversed,
                bId,
                bLength,
                bStart,
                bEnd,
                numberOfSequenceMatches,
                alignmentBlockLength
        );

        reads[aId] = Read(aId, aLength,0, false);

        if (aId != bId) {

            // add b->a
            overlaps.emplace_back(
                    bId,
                    bLength,
                    bStart,
                    bEnd,
                    isReversed,
                    aId,
                    aLength,
                    aStart,
                    aEnd,
                    numberOfSequenceMatches,
                    alignmentBlockLength
            );

            reads[bId] = Read(bId, bLength, 0, false);

        }

    }

    // sort overlaps

    std::sort(overlaps.begin(),overlaps.end());
}

void logOverlaps(const Overlaps &overlaps) {
    for(auto const & overlap:overlaps){
        fprintf(stdout,"%s\n",overlap.toString().c_str());
    }
}
