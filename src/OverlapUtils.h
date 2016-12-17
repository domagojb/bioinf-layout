//
// Created by Domagoj Boros on 04/12/2016.
//

#pragma once


#include "common.h"

void proposeReadTrims(
        ReadTrims &readTrims,
        const Overlaps &overlaps,
        const Params & params,
        bool clipEndings
);

void filterChimeric(Overlaps& overlaps, ReadTrims& readTrims, Params& params);
void filterContained(Overlaps& overlaps, ReadTrims& readTrims, Params& params);
