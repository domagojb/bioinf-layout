//
// Created by Domagoj Boros on 04/12/2016.
//

#pragma once


#include "common.h"
#include "ReadTrim.h"
#include "Overlap.h"
#include "Graph.h"
#include "params.h"

void proposeReadTrims( ReadTrims & readTrims, const Overlaps & overlaps, const Params & params );

void filterChimeric( const Overlaps & overlaps, ReadTrims & readTrims, const Params & params );

void filterContained( Overlaps & overlaps, ReadTrims & readTrims, const Params & params );

void trimReads( Overlaps & overlaps, ReadTrims & readTrims, const Params &params );

void classifyOverlapAndMeasureItsLength( OverlapClassification & overlapClassification,
                                         Edge & edge,
                                         const Overlap overlap,
                                         read_size_t aLength,
                                         read_size_t bLength,
                                         read_size_t maximalOverhangLength,
                                         float mappingLengthRatio );


void filterInternalReads( Overlaps & overlaps, ReadTrims & readTrims, const Params &params );