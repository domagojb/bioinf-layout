//
// Created by Domagoj Boros on 18/12/2016.
//

#include "OverlapUtils.h"
#include "GraphUtils.h"
#include <fstream>
#include <assert.h>

static void isUnitigEnd( GraphEdgeType & graphEdgeType, Vertex & vertexOut, Graph & graph, const Vertex & vertexIn ) {

    const Edges & edges( graph[invertVertex( vertexIn )] );

    size_t undeletedSize( 0 );
    size_t i( 0 );
    size_t undeletedEdgeIdx( 0 );
    for ( const auto & edge : edges ) {
        if ( !edge.del ) {
            ++undeletedSize;
            undeletedEdgeIdx = i;
        }
        ++i;
    }


    if ( undeletedSize == 0 ) {
        graphEdgeType = GraphEdgeType::GRAPH_EDGE_TYPE_TIP;
        return;
    }

    if ( undeletedSize > 1 ) {
        graphEdgeType = GraphEdgeType::GRAPH_EDGE_TYPE_MULTI_OUT;
        return;
    }

    vertexOut = bVertex( edges[undeletedEdgeIdx] );

    undeletedSize = 0;
    for ( const auto & edge : graph[invertVertex( vertexOut )] ) {
        if ( !edge.del ) {
            ++undeletedSize;
        }
    }

    graphEdgeType = undeletedSize != 1 ? GRAPH_EDGE_TYPE_MULTI_NEI : GRAPH_EDGE_TYPE_MERGEABLE;

}

static void extend( std::vector<read_id_t> & readIds,
                    GraphEdgeType & graphEdgeType,
                    Graph & g,
                    const Vertex & v,
                    const Params & params ) {
    size_t tipExtension( params.maximalTipExtension );

    Vertex vertex = v;

    readIds.push_back( vertex.first );
    while ( true ) {
        isUnitigEnd( graphEdgeType, vertex, g, invertVertex( vertex ));
        if ( graphEdgeType != GraphEdgeType::GRAPH_EDGE_TYPE_MERGEABLE ) break;

        readIds.push_back( vertex.first );
        if ( --tipExtension == 0 ) break;
    }
}


void generateGraph( Graph & g, const Overlaps & overlaps, ReadTrims & readTrims, Params & params ) {
    TIMER_START( "Generating a graph..." );

    int edgeCnt = 0;
    for ( const auto & pair: readTrims ) {
        g[makeVertex( pair.first, true )].reserve( 1 );
        g[makeVertex( pair.first, false )].reserve( 1 );
    }


    for ( const auto & o : overlaps ) {
        OverlapClassification c;
        Edge                  e;
        classifyOverlapAndMeasureItsLength( c,
                                            e,
                                            o,
                                            readTrims[o.aId()].length(),
                                            readTrims[o.bId()].length(),
                                            params.maximalOverhangLength,
                                            params.mappingLengthRatio
                                          );
        if ( c == OVERLAP_A_TO_B_OR_B_TO_A ) {
            g[makeVertex( e.aId, e.aIsReversed )].push_back( e );
            edgeCnt++;
        }
    }

    for ( auto & p : g ) {
        std::sort( p.second.begin(), p.second.end(), []( const Edge & a, const Edge & b ) {
                       return ( a.overlapLength < b.overlapLength );
                   }
                 );
    }

    std::cout << "Generated " << edgeCnt << " edges" << std::endl;
    TIMER_END( "Done generating a graph, time passed: " );
}

void filterTransitiveEdges( Graph & g, const Params & params ) {
    TIMER_START( "Filtering transitive edges..." );

#define VACANT 0 // unused vertex
#define INPLAY 1 // used vertex
#define ELIMINATED 2 // eliminated vertex

    std::map<Vertex, char> mark;
    for ( const auto & p : g ) {
        mark[p.first] = VACANT;
        for ( const Edge & vw : p.second ) { mark[bVertex( vw )] = VACANT; }
    }

    int reduceCnt = 0;
    for ( auto & p : g ) {
        for ( const auto & vw : p.second ) { mark[bVertex( vw )] = INPLAY; }

        if ( p.second.size() == 0 ) continue;

        read_size_t longest = p.second[p.second.size() - 1].overlapLength + params.filterTransitiveFuzz;
        for ( const auto & vw : p.second ) {
            Vertex w( bVertex( vw ));
            if ( mark[w] == INPLAY ) {
                for ( const auto & wx : g[w] ) {
                    if ( wx.overlapLength + vw.overlapLength > longest ) break;
                    Vertex x( bVertex( wx ));
                    if ( mark[x] == INPLAY ) mark[x] = ELIMINATED;
                }
            }
        }

        //        for (const auto& vw : p.second) {
        //            Vertex w(vw.bId, vw.bIsReversed);
        //            int i = 0;
        //            for (const auto& wx : g[w]) {
        //                if (wx.overlapLength >= params.filterTransitiveFuzz && i != 0) break;
        //                Vertex x(wx.bId,wx.bIsReversed);
        //                if (mark[x] == INPLAY) mark[x] = ELIMINATED;
        //                i++;
        //            }
        //        }

        for ( auto & vw : p.second ) {
            Vertex w( bVertex( vw ));
            if ( mark[w] == ELIMINATED ) {
                vw.del = true;
                reduceCnt++;
            }
            mark[w] = VACANT;
        }
    }

    cleanGraph( g );

    std::cout << "Reduced " << reduceCnt << " edges" << std::endl;

#undef VACANT
#undef INPLAY
#undef ELIMINATED
}

void logGraph( const Graph & g ) {
    for ( const auto & pair : g ) {
        auto u( pair.first );
        std::cout << u.first << " !"[u.second] << std::endl;
        const auto & edges( pair.second );

        for ( auto const & edge: edges ) {
            std::cout
                    << "\t"
                    << edge.aId
                    << " !"[edge.aIsReversed]
                    << " --"
                    << edge.overlapLength
                    << "--> "
                    << edge.bId
                    << " !"[edge.bIsReversed]
                    << " "
                    << " D"[edge.del]
                    << std::endl;
        }
    }
}

void removeAsymetricEdges( Graph & g ) {
    TIMER_START("Removing assymetric edges...");
    size_t cnt( 0 );

    for ( auto & p : g ) {
        for ( auto & e : p.second ) {
            bool found = false;
            for ( auto & e2 : g[bInvertVertex(e)] ) {
                if ( e2.bId == p.first.first && e2.bIsReversed != p.first.second ) {
                    found = true;
                    break;
                }
            }
            if ( !found ) {
                e.del = true;
                ++cnt;
            }
        }
    }

    cleanGraph( g );

    std::cout << "Removing " << cnt << " asymetric edges" << std::endl;
    TIMER_END("Done with assymetric edges, time elapsed: ");
}

void cleanGraph( Graph & g ) {
    for ( auto & p : g ) {
        Edges newEdges;
        for ( auto & e : p.second ) {
            if ( !e.del ) newEdges.push_back( e );
        }
        p.second.swap( newEdges );
        newEdges.clear();
    }
}

void cutTips( Graph & g, ReadTrims & readTrims, const Params & params ) {
    TIMER_START("Cutting tips...");

    std::vector<read_id_t> readIds;
    size_t cnt( 0 );
    for ( auto & p : g ) {
        Vertex        vertexOut;
        GraphEdgeType graphEdgeType;

        if ( readTrims[p.first.first].del ) continue;

        isUnitigEnd( graphEdgeType, vertexOut, g, p.first );

        if ( graphEdgeType != GRAPH_EDGE_TYPE_TIP ) continue; // not a tip

        readIds.clear();
        extend( readIds, graphEdgeType, g, p.first, params );

        if ( graphEdgeType == GRAPH_EDGE_TYPE_MERGEABLE ) continue;
        ++cnt;

        for ( read_id_t readId: readIds ) {
            readTrims[readId].del = true;

            for ( int i = 0; i < 2; ++i ) {
                for ( auto & edge: g[makeVertex( readId, static_cast<bool>(i))] ) {
                    edge.del = true;

                    for ( auto & edge2: g[ bInvertVertex( edge )] ) {
                        if ( edge2.bId == readId && edge2.bIsReversed != edge.aIsReversed ) {
                            edge2.del = true;
                        }
                    }
                }
            }
        }
    }

    cleanGraph( g );

    std::cout << "Cutting " << cnt << " tips" << std::endl;
    TIMER_END("Done cutting, time passed: ");
}

int countIncoming( Graph & g, Vertex & v ) {
    int cnt = 0;
    for ( const auto & e : g[invertVertex(v)] ) { if ( !e.del ) cnt++; }
    return cnt;
}

template <bool shouldDelete>
static void markEdge(Graph & g, const Vertex & u, const Vertex & v) {
    for ( auto & edge: g[u] ) {
        if ( edge.bId == v.first && edge.bIsReversed == v.second ) {
            edge.del = shouldDelete;
        }
    }
}

static void popBubblesInternal( int & cnt, Graph & g, const Vertex & v, ReadTrims & readTrims ) {
#define D 50000
    std::map<Vertex, int>    distances;
    std::map<Vertex, int>    readCnt;
    std::map<Vertex, int>    unvisitedIncoming;
    std::vector<Vertex>      S;
    std::vector<Vertex>      visitedV;
    std::map<Vertex, Vertex> optPath;

    const Vertex read0(v);

    if(readTrims[v.first].del) return;
    if(g[v].size() < 2) return;

        for ( auto & p2 : g ) {
            distances[p2.first] = std::numeric_limits<int>::max();
            readCnt[p2.first] = 0;
        }

    distances[read0] = 0;

    S.push_back( read0 );
    int pv = 0;

    bool terminate(false);

    do {
        Vertex read(S.back().first,S.back().second);
        S.pop_back();
        int rcnt = readCnt[read];

        size_t i(0);
        for ( auto & edge: g[read]) {
            if ( edge.bId == read0.first ) {
                terminate = true;
                break;
            }

            if ( edge.del ) continue;

            edge.visited = true;

            Vertex b(bVertex(edge));

            if ( distances[read] + edge.overlapLength > D ) break;

            if ( std::find( visitedV.begin(), visitedV.end(), b ) == visitedV.end()) { // not visited
                unvisitedIncoming[b] = countIncoming( g, b );
                ++pv;
                visitedV.push_back( b );

                optPath[b]   = read;
                distances[b] = distances[read] + edge.overlapLength;
            } else {
//                if ( rcnt + 1 > readCnt[b] || ( rcnt + 1 == readCnt[b] && distances[read] + edge.overlapLength > distances[b] )) {
                if (  distances[read] + edge.overlapLength > distances[b] ) {
                    optPath[b] = read;
                }
                if ( rcnt + 1 > readCnt[b] ) readCnt[b] = rcnt + 1;
                if ( distances[read] + edge.overlapLength < distances[b] ) {
                    distances[b] = distances[read] + edge.overlapLength;
                }
            }

            assert( unvisitedIncoming[b] > 0 );
            --unvisitedIncoming[b];
            if ( unvisitedIncoming[b] == 0 ) {
                if ( g[b].size() != 0 ) {
                    S.push_back( b );
                }
                --pv;
            }
            i++;
        }

        if ( i<g[read].size() || S.size() == 0 || terminate) {
            terminate = true;
            break;
        }

    } while ( S.size() > 1 || pv );
//region backtrack
    if ( !terminate && S.size() == 1 && pv == 0 ) {
        cnt++;

        // delete visited vertex and edges
        for ( auto & vv : visitedV ) readTrims[vv.first].del = true;

        for ( auto & p2 : g ) {
            for ( auto & edge: p2.second ) {
                if ( edge.visited ) {
                    edge.del = true;
                    markEdge<true>(g,invertVertex(bVertex(edge)),invertVertex(p2.first));
                }
            }
        }


        Vertex & v = S.back();
        do {
            Vertex & u = optPath[v]; // u -> v
            readTrims[v.first].del = false;
            markEdge<false>(g,u,v);
            markEdge<false>(g,invertVertex(v),invertVertex(u));
            v = u;
        } while ( v != read0 );
    }
//endregion
    for ( auto & p2 : g ) { for ( auto & e : p2.second ) { e.visited = false; }}
#undef D
}

void popBubbles( Graph & g, ReadTrims & readTrims ) {
    TIMER_START("Popping bubbles...");


    int cnt = 0;
    for ( auto & p : g ) {
        if ( p.second.size() < 2 || readTrims[p.first.first].del ) continue;
        int nonDeleted = 0;
        for ( auto & edge: p.second ) {
            if ( !edge.del ) nonDeleted++;
        }

        if ( nonDeleted <= 1 ) continue;

        popBubblesInternal( cnt, g, p.first, readTrims );
    }

    cleanGraph( g );

    std::cout << "Poping " << cnt << " bubles" << std::endl;

#undef D
    TIMER_END("Done popping, time passed: ");
}

bool deleteShortEdges(Graph &g, float r) {
    // drop edges that have r ratio less sequence matches then edge with max seq matches
    int cntShort = 0;
    for (auto &p : g) {
        if (p.second.size() < 2) continue;
        int maxOverlap = 0;
        for (auto &e : p.second) maxOverlap = std::max(maxOverlap, e.numberOfSequenceMatches);

        if (maxOverlap != p.second[0].numberOfSequenceMatches) continue;

        int thres = static_cast<int>(p.second[0].numberOfSequenceMatches * r + .499);

        int i;
        for (i = static_cast<int>(p.second.size()) - 1; i >= 1 && p.second[i].numberOfSequenceMatches < thres; i--);
        for (i = i + 1; i <  p.second.size(); ++i) p.second[i].del = true, ++cntShort;
    }

    cleanGraph(g);
    removeAsymetricEdges(g);
    std::cout << "Removed " << cntShort << " reads" << std::endl;
    return cntShort != 0;
}