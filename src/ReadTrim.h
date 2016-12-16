//
// Created by Ivan Jurin on 12/16/16.
//

#pragma once

class ReadTrim {
public:
    ReadTrim(int begin, int end, bool del) : begin(begin), end(end), del(del) {}
    ReadTrim() : ReadTrim(0,0, false) {}

    const std::string toString() const {
        std::stringstream ss;
        ss<<begin<<"-"<<end<<" "<<"NY"[del];
        return ss.str();
    }

    int begin;
    int end;
    bool del;
};