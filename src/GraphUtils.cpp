//
// Created by Domagoj Boros on 18/12/2016.
//

#include "OverlapUtils.h"
#include "GraphUtils.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <limits>
#include <chrono>
#include <fstream>
#include <assert.h>
#include <iomanip>

static void
isUnitigEnd( GraphEdgeType & graphEdgeType, Vertex & vertexOut, const Graph & graph, const Vertex & vertexIn ) {

    const Edges & edges( graph.at( std::make_pair( vertexIn.first, !vertexIn.second )));

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

    vertexOut = std::make_pair( edges[undeletedEdgeIdx].bId, edges[undeletedEdgeIdx].bIsReversed );

    Vertex vertexOutNeg = std::make_pair( vertexOut.first, !vertexOut.second );

    undeletedSize = 0;
    for ( const auto & edge : graph.at( vertexOutNeg )) {
        if ( !edge.del ) {
            ++undeletedSize;
        }
    }

    graphEdgeType = undeletedSize != 1 ? GRAPH_EDGE_TYPE_MULTI_NEI : GRAPH_EDGE_TYPE_MERGEABLE;

}

static void extend( std::vector<read_id_t> & readIds,
                    GraphEdgeType & graphEdgeType,
                    const Graph & g,
                    const Vertex & v,
                    const Params & params ) {
    size_t tipExtension( params.maximalTipExtension );

    Vertex vertex = v;

    readIds.push_back( vertex.first );
    while ( true ) {
        isUnitigEnd( graphEdgeType, vertex, g, std::make_pair( vertex.first, !vertex.second ));
        //        printf("%d\n",graphEdgeType);
        if ( graphEdgeType != GraphEdgeType::GRAPH_EDGE_TYPE_MERGEABLE ) break;

        readIds.push_back( vertex.first );
        if ( --tipExtension == 0 ) break;
    }
}


void generateGraph( Graph & g, const Overlaps & overlaps, ReadTrims & readTrims, Params & params ) {
    TIMER_START("Generating a graph...");

    int edgeCnt = 0;
    for ( const auto & pair: readTrims ) {
        g[std::make_pair( pair.first, true )].reserve( 1 );
        g[std::make_pair( pair.first, false )].reserve( 1 );
    }


    for ( const auto & o : overlaps ) {
        OverlapClassification c;
        Edge                  e;
        classifyOverlapAndMeasureItsLength( c,
                                            e,
                                            o,
                                            readTrims[ o.aId()].length(),
                                            readTrims[ o.bId()].length(),
                                            params.maximalOverhangLength,
                                            params.mappingLengthRatio
                                          );
        if ( c == OVERLAP_A_TO_B_OR_B_TO_A ) {
            g[std::make_pair( e.aId, e.aIsReversed )].push_back( e );
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
    TIMER_END("Done generating a graph, time passed: ");
}

void filterTransitiveEdges( Graph & g, read_size_t FUZZ ) {
    TIMER_START("Filtering transitive edges...");

#define VACANT 0
#define INPLAY 1
#define ELIMINATED 2

    std::map<std::pair<read_id_t, bool>, char> mark;
    for ( const auto & p : g ) {
        mark[p.first] = VACANT;
        for ( const Edge & vw : p.second ) { mark[std::make_pair( vw.bId, vw.bIsReversed )] = VACANT; }
    }

    int reduceCnt = 0;
    for ( auto & p : g ) {
        for ( const auto & vw : p.second ) { mark[std::make_pair( vw.bId, vw.bIsReversed )] = INPLAY; }

        if ( p.second.size() == 0 ) continue;

        read_size_t longest = p.second[p.second.size() - 1].overlapLength + FUZZ;
        for ( const auto & vw : p.second ) {
            std::pair<read_id_t, bool> w( vw.bId, vw.bIsReversed );
            if ( mark[w] == INPLAY ) {
                for ( const auto & wx : g[w] ) {
                    if ( wx.overlapLength + vw.overlapLength > longest ) break;
                    std::pair<read_id_t, bool> x( wx.bId, wx.bIsReversed );
                    if ( mark[x] == INPLAY ) mark[x] = ELIMINATED;
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

        for ( auto & vw : p.second ) {
            std::pair<read_id_t, bool> w( vw.bId, vw.bIsReversed );
            if ( mark[w] == ELIMINATED ) {
                vw.del = 1;
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


void logGraphToFile( std::ofstream & ofstream, const Graph & g, const ReadTrims & readTrims ) {
//    myfile.open( "/Users/ijurin/Documents/fer/devemestar/bioinf/bioinf-layout/src/1.txt" );
    for ( const auto & pair : g ) {
        auto u( pair.first );
        //        std::cout << u.first << " !"[u.second] << std::endl;
        const auto & edges( pair.second );

        for ( auto const & edge: edges ) {
            //            std::cout
            ofstream
                    //                    << "\t"
                    << std::setw( 5 )
                    << edge.aId
                    << " !"[edge.aIsReversed]
                    << " --"
                    << edge.overlapLength
                    << "--> "
                    << std::setw( 5 )
                    << edge.bId
                    << " !"[edge.bIsReversed]
                    << " "
                    << " D"[edge.del]
                    << std::endl;
        }
    }
    for ( const auto & pair : readTrims ) {
        ofstream
                << std::setw( 5 )
                << pair.first
                << " "
                << pair.second.start
                << " "
                << pair.second.end
                << " "
                << pair.second.del
                << std::endl;
    }
    ofstream.close();
}

void removeAsymetricEdges( Graph & g ) {
    TIMER_START("Removing assymetric edges...");
    size_t cnt( 0 );

    for ( auto & p : g ) {
        for ( auto & e : p.second ) {
            bool found = false;
            for ( auto & e2 : g[std::make_pair( e.bId, !e.bIsReversed )] ) {
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

    size_t cnt( 0 );

    //    auto p(std::make_pair(std::make_pair(198,false),g[std::make_pair(198,false)]));
    for ( auto & p : g ) {
        //        std::cout<<p.first.first<<" !"[p.first.second]<<std::endl;
        Vertex        vertexOut;
        GraphEdgeType graphEdgeType;

        if ( readTrims[p.first.first].del ) continue;

        isUnitigEnd( graphEdgeType, vertexOut, g, p.first );
        //    printf("Not deleted\n");

        if ( graphEdgeType != GRAPH_EDGE_TYPE_TIP ) continue; // not a tip
        //        printf("Is tip\n");

        std::vector<read_id_t> readIds;

        extend( readIds, graphEdgeType, g, p.first, params );

        if ( graphEdgeType == GRAPH_EDGE_TYPE_MERGEABLE ) continue;
        //        printf("Not unitig\n");

        //        if (readIds.size() == 0) continue;
        ++cnt;

        for ( read_id_t readId: readIds ) {
            readTrims[readId].del = true;


            // delete all outgoing edges from u and u' and all reverse edges (if u->v is deleted delete v'->u')

            for ( int i = 0; i < 2; ++i ) {
                for ( auto & edge: g[std::make_pair( readId, static_cast<bool>(i))] ) {
                    edge.del = true;

                    for ( auto & edge2: g[std::make_pair( edge.bId, !edge.bIsReversed )] ) {
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
    for ( const auto & e : g[std::make_pair( v.first, !v.second )] ) { if ( !e.del ) cnt++; }
    return cnt;
}

static void popBubblesInternal( int & cnt, Graph & g, const Vertex & v, ReadTrims & readTrims ) {
#define D 50000
    bool printaj;
    if(v.first == 2939 || v.first == 23472){
//        printf("jej\n");
//        printaj = true;
    }
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

    do {
        Vertex read(S.back().first,S.back().second);
        S.pop_back();
        if(printaj) fprintf(stderr,"From %d%c\n",read.first," !"[read.second]);
        int rcnt = readCnt[read];

        size_t i(0);
        for ( auto & edge: g[read]) {
            if ( edge.bId == read0.first ) {
                goto pop_reset;
            }  // jel se moze napisat ovak il se mora cijeli vertex usporedit && edge.bIsReversed == read0.second
            if ( edge.del ) continue;

            edge.visited = true;
            if(printaj) fprintf(stderr,"\t%d%c -> %d%c\n",edge.aId," !"[edge.aIsReversed],edge.bId," !"[edge.bIsReversed]);

            Vertex b = std::make_pair( edge.bId, edge.bIsReversed );

            if ( distances[read] + edge.overlapLength > D ) break;

            if ( std::find( visitedV.begin(), visitedV.end(), b ) == visitedV.end()) { // not visited
                unvisitedIncoming[b] = countIncoming( g, b );
                ++pv;
                visitedV.push_back( b );

                optPath[b]   = read;
                distances[b] = distances[read] + edge.overlapLength;
            } else { // visited
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



        if ( i<g[read].size() || S.size() == 0 ) {
            goto pop_reset;
        }


    } while ( S.size() > 1 || pv );
//region backtrack
    if ( S.size() == 1 && pv == 0 ) {
        cnt++;
//        std::cout
//                << "Found bubble "
//                << read0.first
//                << " !"[read0.second]
//                << " -> "
//                << S.back().first
//                << " !"[S.back().second]
//                << std::endl;
        std::cout
                << "Found bubble";
//                << read0.first
//                << " !"[read0.second]
//                << " -> "
//                << S.back().first
//                << " !"[S.back().second]
//                << std::endl;

        // delete visited vertex and edges
        for ( auto & vv : visitedV ) {
//            std::cout << "1) Deleting read " << vv.first << std::endl;
            readTrims[vv.first].del = true;
            //                delv++;
        }
        for ( auto & p2 : g ) {
            for ( auto & edge: p2.second ) {
                if ( edge.visited ) {

//                    std::cout
//                            << "2) Deleting edge "
//                            << edge.aId
//                            << " !"[edge.aIsReversed]
//                            << " -> "
//                            << edge.bId
//                            << " !"[edge.bIsReversed]
//                            << std::endl;
                    //                        dele++;
                    edge.del = true;
                    for ( auto & edge2: g[std::make_pair( edge.bId, !edge.bIsReversed )] ) {
                        if ( edge2.bId == edge.aId && edge2.bIsReversed != edge.aIsReversed ) {
//                            std::cout
//                                    << "X) Deleting edge "
//                                    << edge2.aId
//                                    << " !"[edge2.aIsReversed]
//                                    << " -> "
//                                    << edge2.bId
//                                    << " !"[edge2.bIsReversed]
//                                    << "  "
//                                    << std::endl;
                            edge2.del = true;
                        }
                    }
                }
            }
        }


        Vertex & v = S.back();
        bool invert(v.second);
//            std::cout << "Visiting " << v.first << " !"[v.second] << std::endl;
        do {
            Vertex & u = optPath[v]; // u -> v
            std::cout << " "<<v.first<<" !"[v.second^invert];
//            std::cout << "3) Undeleting read " << v.first  << std::endl;
            readTrims[v.first].del = false;
            //                delv--;
            //                dele--;
            for ( auto & edge: g[u] ) {
                if ( edge.bId == v.first && edge.bIsReversed == v.second ) {
//                    std::cout
//                            << "X) Deleting edge "
//                            << edge.aId
//                            << " !"[edge.aIsReversed]
//                            << " -> "
//                            << edge.bId
//                            << " !"[edge.bIsReversed]
//                            << " NOT"
//                            << std::endl;
                    edge.del = false;
                }
            }
            for ( auto & edge: g[std::make_pair( v.first, !v.second )] ) {
                if ( edge.bId == u.first && edge.bIsReversed == !u.second ) {
//                    std::cout
//                            << "X) Deleting edge "
//                            << edge.aId
//                            << " !"[edge.aIsReversed]
//                            << " -> "
//                            << edge.bId
//                            << " !"[edge.bIsReversed]
//                            << " NOT"
//                            << std::endl;
                    edge.del = false;
                }
            }
            v = u;
        } while ( v != read0 );
        std::cout << " "<<v.first<<" !"[v.second^invert];
        std::cout<<std::endl;


        //                cleanGraph( g );
        //            break;
    }
//endregion
pop_reset:
    for ( auto & p2 : g ) { for ( auto & e : p2.second ) { e.visited = false; }}
#undef D
}

void popBubbles( Graph & g, ReadTrims & readTrims ) {
    TIMER_START("Popping bubbles...");


    int cnt = 0;
//    popBubblesInternal( cnt, g, std::make_pair(8758,true), readTrims );
    //    int delv = 0;
    //    int dele = 0;
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

    //    std::cout << "v " << delv << " e " << dele << std::endl;
    std::cout << "Poping " << cnt << " bubles" << std::endl;

#undef D
    TIMER_END("Done popping, time passed: ");
}
