//
// Created by Domagoj Boros on 06/11/2016.
//

#pragma once

#include <string>
#include <sstream>

class Overlap {
public:

    Overlap(
            int aId,
            int aLength,
            int aStart,
            int aEnd,
            bool isReversed,
            int bId,
            int bLength,
            int bStart,
            int bEnd,
            int numberOfSequenceMatches,
            int alignmentBlockLength
    );



    const std::string toStringVerbose() const;

    const std::string toString() const;

    inline bool operator<(const Overlap &rhs) const {
        if (aId_ < rhs.aId_)
            return true;
        if (rhs.aId_ < aId_)
            return false;
        return aStart_ < rhs.aStart_;
    }

private:
    int aId_;
    int aLength_;
    int aStart_;
    int aEnd_;
    bool isReversed_;
    int bId_;
    int bLength_;
    int bStart_;
    int bEnd_;
    int numberOfSequenceMatches_;
    int alignmentBlockLength_;
};
