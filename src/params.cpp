//
// Created by Ivan Jurin on 12/15/16.
//

#include "params.h"

Params getDefaultParams() {
    Params params;
    params.minAllowedMatchSpan               = 2000;
    params.minAllowedNumberOfSequenceMatches = 100;
    params.minimalReadCoverage               = 3;
    params.minimalIdentityFactor             = 0.05f;
    params.maximalOverhangLength             = 1000; //bp
    params.mappingLengthRatio                = 0.8;
    params.minimalOverlap                    = 2000; //bp
    params.maximalTipExtension               = 4; // reads
    params.filterTransitiveFuzz              = 1000;


    params.min_ovlp_drop_ratio = .5;
    params.max_ovlp_drop_ratio = .7;
    params.final_ovlp_drop_ratio = .8;
    params.n_rounds = 2;

    return params;
}
