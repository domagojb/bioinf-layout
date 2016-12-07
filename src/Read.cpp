//
// Created by Domagoj Boros on 04/12/2016.
//

#include "Read.h"

static std::string createReverseComplement(const std::string& sequence) {

    std::string rc;
    rc.reserve(sequence.size());

    for (int i = (int) sequence.size() - 1; i >= 0; --i) {

        char c = sequence[i];

        switch (c) {
            case 'A':
                c = 'T';
                break;
            case 'T':
                c = 'A';
                break;
            case 'C':
                c = 'G';
                break;
            case 'G':
                c = 'C';
                break;
            default:
                break;
        }

        rc.push_back(c);
    }

    return rc;
}

Read::Read(size_t id, const std::string &sequence) :
id_(id), sequence_(sequence) {
    reverseComplement_ = createReverseComplement(sequence);
}

Read::~Read() {
}