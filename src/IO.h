//
// Created by Domagoj Boros on 04/11/2016.
//

#pragma once

#include <vector>

class Overlap;
class Read;

/**
 * Reads overlaps from the given MHAP file.
 *
 * @param path path to MHAP file
 * @param overlaps list of overlaps extracted from the file
 * @return number of overlaps extracted
 */
int readMHAPOverlaps(const std::string& path, std::vector<Overlap* >& overlaps);

/**
 * Reads reads from the given FASTA file.
 *
 * @param path path to FASTA file
 * @param reads list of reads extracted from the file
 * @return number of reads extracted
 */
int readFASTAReads(const std::string& path, std::vector<Read* >& reads);

void writeOverlapsToSIF(const std::string& path, const std::vector<Overlap* >& overlaps);