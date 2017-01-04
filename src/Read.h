//
// Created by Domagoj Boros on 04/12/2016.
//

#pragma once

#include "common.h"
#include <string>
#include <unordered_map>

class Read {
public:
    Read(int id, int length) : id_(id), length_(length) {};

    Read() : Read(0, 0) {};

private:
    // we don't need no del nor aux (if you think you need it JURE wrong)
    const read_id_t id_;
    const read_size_t length_;
};


typedef std::unordered_map<read_id_t, Read>                       Reads;