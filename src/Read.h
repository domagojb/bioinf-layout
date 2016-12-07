//
// Created by Domagoj Boros on 04/12/2016.
//

#pragma once

#include <string>

class Read {
public:
    Read(size_t id, const std::string& sequence);
    ~Read();

    size_t getId() const { return id_; }

    const std::string& sequence() const { return sequence_; }

    const std::string& reverseComplement() const { return reverseComplement_; }

private:
    size_t id_;
    std::string sequence_;
    std::string reverseComplement_;
};
