//
// Created by Ivan Jurin on 12/15/16.
//
#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>

typedef int read_id_t; // read id type (aId..)
typedef int read_size_t; // read size type (aStart, aEnd...)

#include "params.h"
#include "Overlap.h"
#include "Read.h"
#include "ReadTrim.h"
#include "Graph.h"

class Overlap;

class Read;

class ReadTrim;

class Edge;

typedef std::pair<read_id_t, bool> Vertex;

struct VertexHash {
public:
    std::size_t operator()(const Vertex &x) const {
        return x.first << 1 | static_cast<std::size_t >(x.second);
    }
};

typedef std::vector<Overlap> Overlaps;
typedef std::unordered_map<read_id_t, Read> Reads;
typedef std::unordered_map<read_id_t, ReadTrim> ReadTrims;
typedef std::unordered_map<Vertex, std::vector<Edge>, VertexHash> Graph;


enum OverlapClassification {
    OVERLAP_INTERNAL_MATCH,
    OVERLAP_SHORT,
    OVERLAP_A_CONTAINED,
    OVERLAP_B_CONTAINED,
    OVERLAP_A_TO_B,
    OVERLAP_B_TO_A,
};