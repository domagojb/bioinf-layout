//
// Created by Ivan Jurin on 12/15/16.
//
#pragma once

#include <vector>
#include <map>

typedef int read_id_t; // read id type (aId..)
typedef int read_size_t; // read size type (aStart, aEnd...)
#include "params.h"

#include "Overlap.h"
#include "Read.h"
#include "ReadTrim.h"

class Overlap;
class Read;
class ReadTrim;

typedef std::vector<Overlap> Overlaps;
typedef std::map<int,Read> Reads;
typedef std::map<int,ReadTrim> ReadTrims;
