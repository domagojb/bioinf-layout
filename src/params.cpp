//
// Created by Ivan Jurin on 12/15/16.
//

#include "params.h"

Params getDefaultParams() {
    Params params;
    params.minAllowedMatchSpan = 2000;
    params.minAllowedNumberOfSequenceMatches = 100;

    return params;
}
