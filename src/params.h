//
// Created by Ivan Jurin on 12/15/16.
//
#pragma once

typedef struct {

    int minAllowedMatchSpan; // bp
    int minAllowedNumberOfSequenceMatches; // bp
} Params;

Params getDefaultParams();