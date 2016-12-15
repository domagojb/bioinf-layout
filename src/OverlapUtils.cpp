//
// Created by Domagoj Boros on 04/12/2016.
//

#include "Overlap.h"

#include "OverlapUtils.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))


void filterInternalMatches(const std::vector<Overlap *>& source, std::vector<Overlap *>& dest, int o, float r) {

//    for (const auto overlap : source) {
//        int overhang = MIN(overlap->getAStart(), overlap->getBStart()) - MIN(overlap->getALength() - overlap->getAEnd(), overlap->getBLength() - overlap->getBEnd());
//        int maplen = MAX(overlap->getAEnd() - overlap->getAStart(), overlap->getBEnd() - overlap->getBStart());
//        if (overhang > MIN(o, maplen * r)) continue;
//        dest.push_back(overlap);
//    }
}

void filterContained(const std::vector<Overlap *>& contained, std::vector<Overlap *>& noncontained) {

//    for (const auto overlap : contained) {
//        if (overlap->getAStart() <= overlap->getBStart() && overlap->getALength() - overlap->getAEnd() <= overlap->getBLength() - overlap->getBEnd()) continue;
//        if (overlap->getAStart() >= overlap->getBStart() && overlap->getALength() - overlap->getAEnd() >= overlap->getBLength() - overlap->getBEnd()) continue;
//        noncontained.push_back(overlap);
//    }
}

void filterTransitiveEdges(const std::vector<Overlap *> &nonContained, std::vector<Overlap *> &nonTransitive) {

    std::map<int,std::vector<std::pair<int,Overlap *>>> allIngoingEdges;
    std::map<int,std::vector<std::pair<int,Overlap *>>> allOutgoingEdges;


    //todo fill z graph

    for(const auto & vertex_edges:allIngoingEdges){
        std::sort(vertex_edges.second.begin(),vertex_edges.second.end());
    }
    for(const auto & vertex_edges:allOutgoingEdges){
        std::sort(vertex_edges.second.begin(),vertex_edges.second.end());
    }

    for(const auto & firstVertexOutgoingEdges:allOutgoingEdges){
//        int firstVertexId = vertex_edges.first;
//
//        auto const & firstVertexOutgoingEdges(vertex_edges.second);
//
//
//        for (const auto & edge : firstVertexOutgoingEdges){
//            int secondVertexId = edge.first;
//
//            auto const & secondVertexIngoingEdges(ingoingEdges.at(secondVertexId));
//
//            for(const auto & firstEdge:firstVertexOutgoingEdges) {
//                for(const auto & secondEdge:secondVertexIngoingEdges) {
//
//
//                }
//            }
//
//        }

    }



}

//void generateAssemblyGraph(std::vector<Overlap *> overlaps, Graph &graph) {
//
//    for (const auto overlap : overlaps) {
//        if (overlap->getAStart() > overlap->getBStart()){
//            graph.addEdge(overlap,true);
//        }
//    }
//}
