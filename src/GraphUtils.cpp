//
// Created by Domagoj Boros on 18/12/2016.
//

#include "OverlapUtils.h"

#include "GraphUtils.h"

void generateGraph(Graph &g, const Overlaps &overlaps, const ReadTrims& readTrims, Params &params) {

    int edgeCnt = 0;
    for (const auto& o : overlaps) {
        OverlapClassification c;
        Edge e;
        classifyOverlapAndMeasureItsLength(c, e, o, readTrims.at(o.aId()).length(), readTrims.at(o.bId()).length(), params.maximalOverhangLength, params.mappingLengthRatio, params.minimalOverlap);
        if (c == OVERLAP_A_TO_B || c == OVERLAP_B_TO_A) {
            g[o.aId()].push_back(e);
            edgeCnt++;
        }
    }

    for (auto& p : g) {
        std::sort(p.second.begin(), p.second.end(), [](const Edge& a, const Edge& b) {
            return (a.overlapLength < b.overlapLength);
        });
    }

    std::cout << "Generated " << edgeCnt << " edges" << std::endl;
}

void filterTransitiveEdges(Graph& g, read_size_t FUZZ) {

    #define VACANT 0
    #define INPLAY 1
    #define ELIMINATED 2

    std::map<read_id_t, char> mark;
    for (const auto& p : g) {
        mark[p.first] = VACANT;
        for (const auto& vw : p.second) mark[vw.bId] = VACANT;
    }

    int reduceCnt = 0;
    for (auto& p : g) {
        for (const auto& vw : p.second) mark[vw.bId] = INPLAY;

        read_size_t longest = p.second[p.second.size() - 1].overlapLength + FUZZ;
        std::cout << longest << std::endl;
        for (const auto& vw : p.second) {
            read_id_t w = vw.bId;
            if (mark[w] == INPLAY) {
                for (const auto& wx : g[w]) {
                    if (wx.overlapLength + vw.overlapLength > longest) break;
                    read_id_t x = wx.bId;
                    if (mark[x] == INPLAY) mark[x] = ELIMINATED;
                }
            }
        }

//        for (const auto& vw : p.second) {
//            read_id_t w = vw.bId;
//            int i = 0;
//            for (const auto& wx : g[w]) {
//                if (wx.overlapLength >= FUZZ && i != 0) break;
//                read_id_t x = wx.bId;
//                if (mark[x] == INPLAY) mark[x] = ELIMINATED;
//                i++;
//            }
//        }

        for (auto& vw : p.second) {
            read_id_t w = vw.bId;
            if (mark[w] == ELIMINATED) { vw.del = 1; reduceCnt++; }
            mark[w] = VACANT;
        }
    }

    std::cout << "Reduced " << reduceCnt << " edges" << std::endl;

    #undef VACANT
    #undef INPLAY
    #undef ELIMINATED
}
