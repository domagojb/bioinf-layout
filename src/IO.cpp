//
// Created by Domagoj Boros on 04/11/2016.
//

#include <iostream>
#include <fstream>

#include "Overlap.h"
#include "Read.h"

#include "IO.h"

int readMHAPOverlaps(const std::string &path, std::vector<Overlap *>& overlaps, std::vector<Read*> reads) {

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
        overlaps.push_back(new Overlap(reads[aId-1], reads[bId-1], aId, bId, !arc, as, ae, al, !brc, bs, be, bl));
        numoverlaps++;
    }

    return numoverlaps;
}

int readFASTAReads(const std::string& path, std::vector<Read *>& reads) {

    #define BUFFER_SIZE 4096

    FILE* f = fopen(path.c_str(), "r");

    std::string name;
    std::string sequence;

    char* buffer = new char[BUFFER_SIZE];

    bool isName = false;
    bool createRead = false;

    size_t idx = 0;

    while (!feof(f)) {

        size_t readLen = fread(buffer, sizeof(char), BUFFER_SIZE, f);

        for (int i = 0; i < readLen; ++i) {

            if (buffer[i] == '>') {

                if (createRead) {
                    reads.push_back(new Read(idx++, sequence));
                }

                name.clear();
                sequence.clear();

                createRead = true;
                isName = true;

            } else {
                switch (buffer[i]) {
                    case '\r':
                        break;
                    case '\n':
                        isName = false;
                        break;
                    default:
                        isName ? name += buffer[i] : sequence += buffer[i];
                        break;
                }
            }
        }
    }

    reads.push_back(new Read(idx, sequence));

    delete[] buffer;
    fclose(f);

    return (int) reads.size();
    #undef BUFFER_SIZE
}

void writeOverlapsToSIF(const std::string& path, const std::vector<Overlap* >& overlaps) {

    std::ofstream os;
    os.open(path);

    char edgeType[2] = {0, 0};
    for (const auto overlap : overlaps) {
        edgeType[0] = (overlap->isBrc()) ? 'a' : 'b';
        os << overlap->getAId() << " " << edgeType << " " << overlap->getBId() << std::endl;
    }

    os.flush();
    os.close();
}
