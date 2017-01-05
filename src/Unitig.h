//
// Created by Ivan Jurin on 1/4/17.
//
#pragma once

#include <deque>
#include "Graph.h"
#include <string>

typedef std::pair<Vertex, read_size_t> UnitigRead;
typedef std::deque<UnitigRead>         UnitigReads;

class Unitig {
public:
    Unitig( Vertex start, Vertex end, read_size_t length, bool isCircular, const UnitigReads & reads ) :
            start( start ),
            end( end ),
            length( length ),
            isCircular( isCircular ),
            reads( reads ) {
    }

    Vertex      start;
    Vertex      end;
    read_size_t length;
    bool        isCircular;
    UnitigReads reads;
    std::string sequence;
};

typedef std::vector<Unitig> Unitigs;
