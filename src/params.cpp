//
// Created by Ivan Jurin on 12/15/16.
//

#include "params.h"
#include "common.h"

Params getDefaultParams() {
    Params params;
    params.minAllowedMatchSpan = 2000;
    params.minAllowedNumberOfSequenceMatches = 100;
    params.minimalReadCoverage = 3;
    params.minimalIdentityFactor = 0.05f;

    params.maxOverhang = 1000;

    return params;
}
