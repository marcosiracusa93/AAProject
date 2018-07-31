//
// Created by Marco Siracusa on 5/28/18.
//

#include <iostream>
#include "Pea_find_scc3_old.hpp"

unsigned long long min_sp_old = 0;

unsigned long long get_sp_old(void) {
    volatile unsigned long long var = 0;
    return (unsigned long long) &var + 24;
}

void update_max_sp_old(void) {
    min_sp_old = std::min(min_sp_old, get_sp_old());
}


Pea_find_scc3_old::Pea_find_scc3_old(const BaseGraph &baseGraph, unsigned int g_numVertices, unsigned int g_numEdges) :
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

void Pea_find_scc3_old::run() {

    unsigned long long init_sp = 0;
    if (TRACK_STACK_CONSUMPTION) {
        init_sp = get_sp_old();
        min_sp_old = init_sp;
    }

    typedef boost::graph_traits<SCC3Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    for (vp = boost::vertices(scc3Graph); vp.first != vp.second; ++vp.first) {
        if (boost::get(rIndex, *vp.first) == 0) {
            boost::property_map<SCC3Graph, boost::vertex_index_t>::type vertex_id = boost::get(boost::vertex_index,
                                                                                               scc3Graph);
            visit(boost::get(vertex_id, *vp.first));
        }
    }

    if (TRACK_STACK_CONSUMPTION) {
        update_max_sp_old();
        stack_dimension = init_sp - min_sp_old;
    }
}

void Pea_find_scc3_old::visit(int v) {
    beginVisiting(v);

    while (!vS.isEmptyFront()) {
        visitLoop();
    }

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp_old();
}

void Pea_find_scc3_old::visitLoop() {
    int v = vS.topFront();
    int i = iS.topFront();

    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);

    typedef boost::graph_traits<SCC3Graph> GraphTraits;
    typename GraphTraits::out_edge_iterator out_i, out_end;
    typename GraphTraits::edge_descriptor e;

    // Count out-edges
    int out_count = 0;
    for (std::tie(out_i, out_end) = boost::out_edges(v, scc3Graph); out_i != out_end; ++out_i) {
        out_count++;
    }

    // Continue traversing out-edges until none left.
    while (i <= out_count) {
        // Continuation
        if (i > 0) {
            // Update status for previously traversed out-edge
            finishEdge(v, i - 1);
        }
        if (i < out_count && beginEdge(v, i)) {
            return;
        }
        i = i + 1;
    }

    // Finished traversing out edges, update component info
    finishVisiting(v);

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp_old();
}

void Pea_find_scc3_old::beginVisiting(int v) {
    // First time this node encountered
    vS.pushFront(v);
    iS.pushFront(0);

    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);
    isRoot[v] = true;
    rIndex[v] = index;
    index = index + 1;

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp_old();
}

void Pea_find_scc3_old::finishVisiting(int v) {
    // Take this vertex off the call stack
    vS.popFront();
    iS.popFront();

    // Update component information
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);
    if (isRoot[v]) {
        index = index - 1;
        while (!vS.isEmptyBack() && rIndex[v] <= rIndex[vS.topBack()]) {
            int w = vS.popBack();
            rIndex[w] = c;
            index = index - 1;
        }
        rIndex[v] = c;
        c = c - 1;
    } else {
        vS.pushBack(v);
    }

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp_old();
}

bool Pea_find_scc3_old::beginEdge(int v, int k) {
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);

    unsigned int w = getTargetVertex(v, k);
    if (rIndex[w] == 0) {
        iS.popFront();
        iS.pushFront(k + 1);
        beginVisiting(w);

        if (TRACK_STACK_CONSUMPTION)
            update_max_sp_old();
        return true;
    } else {
        if (TRACK_STACK_CONSUMPTION)
            update_max_sp_old();
        return false;
    }

}

void Pea_find_scc3_old::finishEdge(int v, int k) {
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);

    int w = getTargetVertex(v, k);

    if (rIndex[w] < rIndex[v]) {
        rIndex[v] = rIndex[w];
        isRoot[v] = false;
    }

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp_old();
}

unsigned int Pea_find_scc3_old::getTargetVertex(int v, int k) {
    typedef boost::graph_traits<SCC3Graph> GraphTraits;
    typename GraphTraits::out_edge_iterator out_i, out_end;
    typename GraphTraits::edge_descriptor e;

    unsigned int w = 0;

    // Count out-endges
    int out_count = 0;
    for (std::tie(out_i, out_end) = boost::out_edges(v, scc3Graph); out_i != out_end; ++out_i) {
        if (k == out_count) {
            w = boost::target(*out_i, scc3Graph);
        }

        out_count++;
    }

    if (TRACK_STACK_CONSUMPTION)
        update_max_sp_old();

    return w;
}

void Pea_find_scc3_old::getSCCResult(unsigned int *scc3) {

    int i = 0;

    typedef boost::graph_traits<SCC3Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    for (vp = boost::vertices(scc3Graph), i = 0; vp.first != vp.second; ++vp.first, i++) {
        scc3[i] = boost::get(rIndex, *vp.first);
    }
}

unsigned long long Pea_find_scc3_old::getStackDimension() {
    return stack_dimension;
}