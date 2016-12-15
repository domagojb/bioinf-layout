//
// Created by Domagoj Boros on 04/12/2016.
//

#pragma once

#include <string>

class Read {
public:
    Read(int id, int length, int aux, bool del) : id_(id), length_(length), aux_(aux), del_(del) {};
    Read() : Read(0,0,0, false) {};

private:
    int id_;
    int length_;
    int aux_;
    bool del_;
};
