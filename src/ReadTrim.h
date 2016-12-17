//
// Created by Ivan Jurin on 12/16/16.
//

#pragma once

class ReadTrim {
public:
    ReadTrim(int begin, int end, bool del) : start(begin), end(end), del(del) {}
    ReadTrim() : ReadTrim(0,0, false) {}

    const std::string toString() const {
        std::stringstream ss;
        ss<<start<<"-"<<end<<" "<<"NY"[del];
        return ss.str();
    }

    int start;
    int end;
    bool del;
};