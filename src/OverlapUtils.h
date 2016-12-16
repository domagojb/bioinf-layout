//
// Created by Domagoj Boros on 04/12/2016.
//

#pragma once


#include "common.h"

void proposeReadTrims(
        ReadTrims &readTrims,
        int minimalReadCoverage,
        float minimalIdentityFactor,
        int endClipping,
        const Overlaps &overlaps
);
