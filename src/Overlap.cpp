//
// Created by Domagoj Boros on 06/11/2016.
//

#include "Overlap.h"

Overlap::Overlap(int aId, int aLength, int aStart, int aEnd, bool isReversed, int bId, int bLength, int bStart,
                 int bEnd, int numberOfSequenceMatches, int alignmentBlockLength) :
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
{}

const std::string Overlap::toStringVerbose() const {
    std::stringstream ss;
    ss<<"Overlap{\n";
    ss<<"\taId:\t"<<aId_<<",\n";
    ss<<"\taStart:\t"<<aStart_<<",\n";
    ss<<"\taEnd:\t"<<aEnd_<<",\n";
    ss<<"\taLength:\t"<<aLength_<<",\n";

    ss<<"\tRelativeStrand:\t"<<"-+"[isReversed_]<<",\n";

    ss<<"\tbId:\t"<<bId_<<",\n";
    ss<<"\tbStart:\t"<<bStart_<<",\n";
    ss<<"\tbEnd:\t"<<bEnd_<<",\n";
    ss<<"\tbLength:\t"<<bLength_<<",\n";

    ss<<"\tnumberOfSequenceMatches:\t"<<numberOfSequenceMatches_<<",\n";
    ss<<"\talignmentBlockLength:\t"<<alignmentBlockLength_<<",\n";
    ss<<"}\n";
    return ss.str();
}


const std::string Overlap::toString() const {
    std::stringstream ss;
    ss<<aId_<<"\t";
    ss<<aStart_<<"\t";
    ss<<aEnd_<<"\t";
    ss<<aLength_<<"\t";

    ss<<"-+"[isReversed_]<<"\t";

    ss<<bId_<<"\t";
    ss<<bStart_<<"\t";
    ss<<bEnd_<<"\t";
    ss<<bLength_<<"\t";

    ss<<numberOfSequenceMatches_<<"\t";
    ss<<alignmentBlockLength_;
    return ss.str();
}
