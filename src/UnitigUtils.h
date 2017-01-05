//
// Created by Ivan Jurin on 1/4/17.
//
#pragma once

#include "Unitig.h"
#include "ReadTrim.h"

void generateUnitigs( Unitigs & unitigs, Graph const & g, ReadTrims const & readTrims );

void assignSequencesToUnitigs( Unitigs & unitigs, const ReadTrims & readTrims, const std::string pathToFASTA );

void logUnitigs( const Unitigs & unitigs, const ReadTrims & readTrims );

