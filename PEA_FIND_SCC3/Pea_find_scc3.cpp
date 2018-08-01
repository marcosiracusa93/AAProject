//
// Created by Marco Siracusa on 5/28/18.
//

#include <iostream>
#include "Pea_find_scc3.hpp"

#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/graph/graphml.hpp>

unsigned long long min_sp = 0;

unsigned long long get_sp(void) {
    volatile unsigned long long var = 0;
    return (unsigned long long) &var + 24;
}

void update_max_sp(void) {
    min_sp = std::min(min_sp, get_sp());
}


Pea_find_scc3::Pea_find_scc3(unsigned int g_numVertices, unsigned int g_numEdges) :
        scc3Graph(g_numVertices), vS(g_numEdges), iS(g_numEdges),
        index(1), c(g_numVertices - 1), stack_dimension(0) {

    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = get(vertex_isRoot, scc3Graph);

    // Randomize the graph
    scc3Graph.clear();
    boost::mt19937 rng;
    rng.seed(uint32_t(time(0)));
    boost::generate_random_graph(scc3Graph, g_numVertices, g_numEdges, rng, true, true);

    // Graph initialization
    for (int i = 0; i < g_numVertices; i++) {
        boost::put(rIndex, i, 0);
        boost::put(isRoot, i, false);
    }
}

Pea_find_scc3::Pea_find_scc3(const BaseGraph &baseGraph, unsigned int g_numVertices, unsigned int g_numEdges) :
        scc3Graph(g_numVertices), vS(g_numEdges), iS(g_numEdges),
        index(1), c(g_numVertices - 1), stack_dimension(0) {

    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = get(vertex_isRoot, scc3Graph);

    typedef boost::graph_traits<BaseGraph>::edge_iterator edge_iter;
    std::pair<edge_iter, edge_iter> ep;
    for (ep = boost::edges(baseGraph); ep.first != ep.second; ++ep.first) {
        boost::add_edge(boost::source(*ep.first, baseGraph), boost::target(*ep.first, baseGraph), scc3Graph);
    }

    // Graph initialization
    for (int i = 0; i < g_numVertices; i++) {
        boost::put(rIndex, i, 0);
        boost::put(isRoot, i, false);
    }
}

void Pea_find_scc3::run() {

    unsigned long long init_sp = 0;
    if (TRACK_STACK_CONSUMPTION) {
        init_sp = get_sp();
        min_sp = init_sp;
    }

    typedef boost::graph_traits<SCC3Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    for (vp = boost::vertices(scc3Graph); vp.first != vp.second; ++vp.first) {
        if (boost::get(rIndex, *vp.first) == 0) {
            boost::property_map<SCC3Graph, boost::vertex_index_t>::type vertex_id = boost::get(boost::vertex_index,
                                                                                               scc3Graph);
            visit(*vp.first);
        }
    }

    if (TRACK_STACK_CONSUMPTION) {
        update_max_sp();
        stack_dimension = init_sp - min_sp;
    }
}

void Pea_find_scc3::visit(Vertex v) {
    beginVisiting(v);

    while (!vS.isEmptyFront()) {
        visitLoop();
    }

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp();
}

void Pea_find_scc3::visitLoop() {
    Vertex v = vS.topFront();
    int i = iS.topFront();

    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);

    typedef boost::graph_traits<SCC3Graph> GraphTraits;
    typename GraphTraits::out_edge_iterator out_i, out_end;
    typename GraphTraits::edge_descriptor e;

    typename boost::graph_traits<BaseGraph>::degree_size_type out_count;
    out_count = boost::out_degree(v, scc3Graph);

    boost::graph_traits<BaseGraph>::out_edge_iterator it, it_end;
    boost::tie(it, it_end) = boost::out_edges(v, scc3Graph);

    /*
    // Count out-edges
    int out_count = 0;
    for (std::tie(out_i, out_end) = boost::out_edges(v, scc3Graph); out_i != out_end; ++out_i) {
        out_count++;
    }
*/

    // Continue traversing out-edges until none left.
    while (i <= out_count) {
        // Continuation
        if (i > 0) {
            // Update status for previously traversed out-edge
            finishEdge(v, i - 1, it);
        }
        if (i < out_count && beginEdge(v, i, it)) {
            return;
        }
        i++;
    }

    // Finished traversing out edges, update component info
    finishVisiting(v);

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp();
}

void Pea_find_scc3::beginVisiting(Vertex v) {
    // First time this node encountered
    vS.pushFront(v);
    iS.pushFront(0);

    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);
    isRoot[v] = true;
    rIndex[v] = index;
    index++;

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp();
}

void Pea_find_scc3::finishVisiting(Vertex v) {
    // Take this vertex off the call stack
    vS.popFront();
    iS.popFront();

    // Update component information
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);
    if (isRoot[v]) {
        index--;
        while (!vS.isEmptyBack() && rIndex[v] <= rIndex[vS.topBack()]) {
            Vertex vv = vS.popBack();
            rIndex[vv] = c;
            index--;
        }
        rIndex[v] = c;
        c--;
    } else {
        vS.pushBack(v);
    }

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp();
}

bool Pea_find_scc3::beginEdge(Vertex v, int k, boost::graph_traits<BaseGraph>::out_edge_iterator it) {
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);

    Vertex vv = target(*(it + k), scc3Graph);

    if (rIndex[vv] == 0) {
        iS.popFront();
        iS.pushFront(k + 1);
        beginVisiting(vv);

        if (TRACK_STACK_CONSUMPTION)
            update_max_sp();
        return true;
    } else {
        if (TRACK_STACK_CONSUMPTION)
            update_max_sp();
        return false;
    }

}

void Pea_find_scc3::finishEdge(Vertex v, int k, boost::graph_traits<BaseGraph>::out_edge_iterator it) {
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);

    Vertex vv = target(*(it + k), scc3Graph);

    if (rIndex[vv] < rIndex[v]) {
        rIndex[v] = rIndex[vv];
        isRoot[v] = false;
    }

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp();
}

void Pea_find_scc3::getSCCResult(unsigned int *scc3) {

    int i = 0;

    typedef boost::graph_traits<SCC3Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    for (vp = boost::vertices(scc3Graph), i = 0; vp.first != vp.second; ++vp.first, i++) {
        scc3[i] = boost::get(rIndex, *vp.first);
    }
}

unsigned long long Pea_find_scc3::getStackDimension() {
    return stack_dimension;
}