#pragma once

#include "common.h"

class Edge {

public:
    read_size_t aId;
    read_size_t bId;
    bool aIsReversed;
    bool bIsReversed;
    read_size_t overlapLength;
    bool del;
    read_size_t numberOfSequenceMatches;
    float numberOfSequenceMatchesRatio;
    bool visited;
};