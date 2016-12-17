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
        os<< overlap.aId()<<" "<<"stipe"<<" "<<overlap.bId()<<std::endl;
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
    read_id_t aId, bId;
    read_size_t aStart, aEnd, aLength;
    read_size_t bStart, bEnd, bLength;
    char relativeStrand; // - or +

    read_size_t numberOfSequenceMatches; // the number of sequence matches
    read_size_t alignmentBlockLength; // the total number of sequence matches, mismatches, insertions and deletions in the alignment

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

        read_size_t aMatchSpan(aEnd - aStart);
        read_size_t bMatchSpan(bEnd - bStart);

        if (aMatchSpan < params.minAllowedMatchSpan) continue;
        if (bMatchSpan < params.minAllowedMatchSpan) continue;
        if (numberOfSequenceMatches < params.minAllowedNumberOfSequenceMatches) continue;


        bool isReversed(relativeStrand == '-');

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

    std::cout << "Read " << overlaps.size() << " overlaps" << std::endl;
    std::cout << "Read " << reads.size() << " reads" << std::endl;
}

void logOverlaps(const Overlaps &overlaps) {

    fprintf(stdout,"aId\taLen\taS\taE\tor\tbLen\tbS\tbE\tml\tbl\n");

    for(auto const & overlap:overlaps){
        fprintf(stdout,"%s\n",overlap.toString().c_str());
    }
}

void logTrimmedOverlap(const Overlap &overlap, const ReadTrims &readTrims) {

    const ReadTrim & aTrim(readTrims.at(overlap.aId()));
    const ReadTrim & bTrim(readTrims.at(overlap.bId()));

    fprintf(stdout,"%05d:%05d-%05d\t%5d\t%5d\t%5d\t%c\t%05d:%05d-%05d\t%5d\t%5d\t%5d\t%5d\t%5d\t255\n",overlap.aId(), aTrim.start + 1, aTrim.end, aTrim.length(), overlap.aStart(), overlap.aEnd(),
            "+-"[overlap.isReversed()], overlap.bId(), bTrim.start + 1, bTrim.end, bTrim.length(), overlap.bStart(), overlap.bEnd(), overlap.numberOfSequenceMatches(), overlap.alignmentBlockLength());
}

void logTrimmedOverlaps(const Overlaps &overlaps, const ReadTrims &readTrims) {

    fprintf(stdout,"aId\taLen\taS\taE\tor\tbLen\tbS\tbE\tml\tbl\n");

    for(auto const & overlap:overlaps){
        logTrimmedOverlap(overlap,readTrims);
    }
}
