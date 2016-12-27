//
// Created by Domagoj Boros on 18/12/2016.
//

#include "OverlapUtils.h"

#include "GraphUtils.h"

void generateGraph(Graph &g, const Overlaps &overlaps, const ReadTrims &readTrims, Params &params) {

    int edgeCnt = 0;
    for (const auto &o : overlaps) {
        OverlapClassification c;
        Edge e;
        classifyOverlapAndMeasureItsLength(c, e, o, readTrims.at(o.aId()).length(), readTrims.at(o.bId()).length(),
                                           params.maximalOverhangLength, params.mappingLengthRatio,
                                           params.minimalOverlap);
        if (c == OVERLAP_A_TO_B || c == OVERLAP_B_TO_A) {
            g[std::make_pair(e.aId, e.aIsReversed)].push_back(e);
            edgeCnt++;
        }
    }

    for (auto &p : g) {
        std::sort(p.second.begin(), p.second.end(), [](const Edge &a, const Edge &b) {
            return (a.overlapLength < b.overlapLength);
        });
    }

    std::cout << "Generated " << edgeCnt << " edges" << std::endl;
}

void filterTransitiveEdges(Graph &g, read_size_t FUZZ) {

#define VACANT 0
#define INPLAY 1
#define ELIMINATED 2

    std::map<std::pair<read_id_t, bool>, char> mark;
    for (const auto &p : g) {
        mark[p.first] = VACANT;
        for (const Edge &vw : p.second) mark[std::make_pair(vw.bId, vw.bIsReversed)] = VACANT;
    }

    int reduceCnt = 0;
    for (auto &p : g) {
        for (const auto &vw : p.second) mark[std::make_pair(vw.bId, vw.bIsReversed)] = INPLAY;

        if(p.second.size() == 0) continue;

        read_size_t longest = p.second[p.second.size() - 1].overlapLength + FUZZ;
        for (const auto &vw : p.second) {
            std::pair<read_id_t, bool> w(vw.bId, vw.bIsReversed);
            if (mark[w] == INPLAY) {
                for (const auto &wx : g[w]) {
                    if (wx.overlapLength + vw.overlapLength > longest) break;
                    std::pair<read_id_t, bool> x(wx.bId, wx.bIsReversed);
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

        for (auto &vw : p.second) {
            std::pair<read_id_t, bool> w (vw.bId, vw.bIsReversed);
            if (mark[w] == ELIMINATED) {
                vw.del = 1;
                reduceCnt++;
            }
            mark[w] = VACANT;
        }
    }

    for (auto& p : g) {
        std::vector<Edge> newEdges;
        for (auto& e : p.second) {
            if (!e.del) newEdges.push_back(e);
        }
        p.second.swap(newEdges);
        newEdges.clear();
    }

    std::cout << "Reduced " << reduceCnt << " edges" << std::endl;

#undef VACANT
#undef INPLAY
#undef ELIMINATED
}

void logGraph(const Graph &g) {
    for (const auto &pair : g) {
        auto u(pair.first);
        std::cout << u.first << " !"[u.second] << std::endl;
        const auto &edges(pair.second);

        for (auto const &edge: edges) {
            std::cout << "\t" << edge.aId << " !"[edge.aIsReversed] << " --" << edge.overlapLength << "--> " << edge.bId
                      << " !"[edge.bIsReversed] << " " << " D"[edge.del] << std::endl;
        }
    }
}

void removeAsymetricEdges(Graph &g) {

    int cnt = 0;
    for (auto& p : g) {
        for (auto& e : p.second) {
            bool found = false;
            for (auto& e2 : g[std::make_pair(e.bId, !e.bIsReversed)]) {
                if (e2.bId == p.first.first && e2.bIsReversed != p.first.second) {
                    found = true;
                    break;
                }
            }
            if (!found) e.del = true, cnt++;
        }
    }

    std::cout << "Removing " << cnt << " asymetric edges" << std::endl;
}
