#include <vector>
#include <map>
#include "Overlap.h"
#include "Read.h"
//
//class Vertex {
//public:
//    std::vector<Overlap *> incomingEdges;
//    std::vector<Overlap *> outcomingEdges;
//    Read * read;
//
//
//};
//


typedef int index_t;

typedef std::pair<index_t,index_t> pair;

//class Graph {
//public:
//
//
////    std::vector<Edge> edges;
//
//    std::map<pair,const Overlap *> _overlapsMap;
//    std::map<index_t,const Read *> _readsMap;
//
//    std::map<index_t,std::map<index_t,index_t>> _outgoingEdgesMap;
//    std::map<index_t,std::map<index_t,index_t>> _incomingEdgesMap;
//
//
////    std::map<Vertex, std::vector<Edge>> vertexEdgeMap;
//
//    void addEdge(Overlap * overlap, bool sameDirection= true){
////        _readsMap[overlap->getAId()] =overlap->getAread();
////        _readsMap[overlap->getBId()] =overlap->getBread();
////
////
////        pair direction;
////
////        if(sameDirection){
////            direction = pair(overlap->getAId(), overlap->getBId());
////        } else {
////            direction = pair(overlap->getBId(),overlap->getAId());
////        }
////
////        _overlapsMap[direction] = overlap;
////
////
////        _outgoingEdgesMap[direction.first][direction.second] = overlap.get
//
//    }
//
//};
