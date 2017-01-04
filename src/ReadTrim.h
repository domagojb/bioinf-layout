//
// Created by Ivan Jurin on 12/16/16.
//

#pragma once

#include <sstream>
#include <unordered_map>
#include "common.h"

class ReadTrim {
public:
    ReadTrim(read_size_t begin, read_size_t end, bool del) : start(begin), end(end), del(del) {}

    ReadTrim() : ReadTrim(0, 0, false) {}

    const std::string toString() const {
        std::stringstream ss;
        ss << start << "-" << end << " " << " Y"[del];
        return ss.str();
    }

    read_size_t length() const { return end - start; }

    read_size_t start;
    read_size_t end;
    bool del;
};
typedef std::unordered_map<read_id_t, ReadTrim>                   ReadTrims;