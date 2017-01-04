//
// Created by Ivan Jurin on 1/4/17.
//
#pragma once


#include <unordered_set>
#include "Unitig.h"
#include "ReadTrim.h"

void logUnitigs( const Unitigs & unitigs, const ReadTrims & readTrims );

void generateUnitigs( Unitigs & unitigs, Graph const & g, ReadTrims const & readTrims );

