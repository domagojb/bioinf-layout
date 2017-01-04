//
// Created by Domagoj Boros on 06/11/2016.
//

#pragma once

#include <string>
#include <sstream>
#include <vector>
#include "common.h"



enum OverlapClassification {
    OVERLAP_INTERNAL_MATCH,
    OVERLAP_SHORT,
    OVERLAP_A_CONTAINED,
    OVERLAP_B_CONTAINED,
    OVERLAP_A_TO_B,
    OVERLAP_B_TO_A,
};



class Overlap {
public:

    Overlap(
            read_id_t aId,
            read_size_t aLength,
            read_size_t aStart,
            read_size_t aEnd,
            bool isReversed,
            read_id_t bId,
            read_size_t bLength,
            read_size_t bStart,
            read_size_t bEnd,
            read_size_t numberOfSequenceMatches,
            read_size_t alignmentBlockLength
    ):
            aId_(aId),
            aLength_(aLength),
            aStart_(aStart),
            aEnd_(aEnd),
            isReversed_(isReversed),
            bId_(bId),
            bLength_(bLength),
            bStart_(bStart),
            bEnd_(bEnd),
            numberOfSequenceMatches_(numberOfSequenceMatches),
            alignmentBlockLength_(alignmentBlockLength)
    {};



    const std::string toStringVerbose() const;

    const std::string toString() const;

    inline bool operator<(const Overlap &rhs) const {
        if (aId_ < rhs.aId_)
            return true;
        if (rhs.aId_ < aId_)
            return false;
        return aStart_ < rhs.aStart_;
    }

    read_id_t aId() const {return aId_;}
    read_size_t aLength() const {return aLength_;}
    read_size_t aStart() const {return aStart_;}
    read_size_t aEnd() const {return aEnd_;}

    bool isReversed() const {return isReversed_;}

    read_id_t bId() const {return bId_;}
    read_size_t bLength() const {return bLength_;}
    read_size_t bStart() const {return bStart_;}
    read_size_t bEnd() const {return bEnd_;}

    read_size_t numberOfSequenceMatches() const { return numberOfSequenceMatches_;}
    read_size_t alignmentBlockLength() const { return alignmentBlockLength_;}

    read_size_t aSpan() const { return aEnd_ - aStart_; }
    read_size_t bSpan() const { return bEnd_ - bStart_; }


private:
    read_id_t aId_;
    read_size_t aLength_;
    read_size_t aStart_;
    read_size_t aEnd_;
    bool isReversed_;
    read_id_t bId_;
    read_size_t bLength_;
    read_size_t bStart_;
    read_size_t bEnd_;
    read_size_t numberOfSequenceMatches_;
    read_size_t alignmentBlockLength_;
};

typedef std::vector<Overlap>                                      Overlaps;

