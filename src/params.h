//
// Created by Ivan Jurin on 12/15/16.
//
#pragma once

#include "common.h"
#include <cstddef>

typedef struct {

    read_size_t minAllowedMatchSpan; // bp
    read_size_t minAllowedNumberOfSequenceMatches; // bp
    read_size_t minimalReadCoverage; // bp
    float minimalIdentityFactor;

    read_size_t maximalOverhangLength; // bp
    float mappingLengthRatio;
    read_size_t minimalOverlap; // bp
    std::size_t maximalTipExtension; // reads

} Params;

Params getDefaultParams();