//
// Created by Ivan Jurin on 12/15/16.
//
#pragma once

#include "common.h"

typedef struct {

    read_size_t minAllowedMatchSpan; // bp
    read_size_t minAllowedNumberOfSequenceMatches; // bp
    read_size_t minimalReadCoverage; //bp
    float minimalIdentityFactor;
} Params;

Params getDefaultParams();