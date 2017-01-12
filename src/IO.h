//
// Created by Domagoj Boros on 04/11/2016.
//

#pragma once

#include <vector>
#include "common.h"
#include "params.h"
#include "Overlap.h"
#include "Graph.h"
#include "ReadTrim.h"
#include "Unitig.h"

void writeGraphToSIF( const std::string & path, const Graph & graph );

/**
 * Reads overlaps from the given PAF file.
 *
 * @param path path to PAF file
 * @param overlaps list of overlaps extracted from the file
 */
void loadPAF( Overlaps & overlaps, const std::string & path, const Params & params );

void logOverlaps( const Overlaps & overlaps );

void logTrimmedOverlap( const Overlap & overlap, const ReadTrims & readTrims );

void logTrimmedOverlaps( const Overlaps & overlaps, const ReadTrims & readTrims );


void unitigsToFASTA( const std::string & outputPath, const Unitigs & unitigs );

void convertPAFtoDIM( const std::string & pathPAF, const std::string & pathDIM );

void loadDIM( Overlaps & overlaps, const std::string & path, const Params & params );
