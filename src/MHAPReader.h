//
// Created by Domagoj Boros on 04/11/2016.
//

#pragma once

#include <vector>

class Overlap;

class MHAPReader {
public:
    /**
     * Reads overlaps from the given MHAP file.
     *
     * @param path path to MHAP file
     * @param overlaps list of overlaps extracted from the file
     * @return number of overlaps extracted
     */
    static int read(const std::string& path, std::vector<Overlap>& overlaps);
};
