//
// Created by Marco Siracusa on 5/28/18.
//

#include <iostream>
#include "Pea_find_scc3.hpp"


Pea_find_scc3::Pea_find_scc3(const BaseGraph &baseGraph) : scc3Graph(numVertices), vS(numEdges), iS(numEdges) {
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = get(vertex_isRoot, scc3Graph);

    typedef boost::graph_traits<BaseGraph>::edge_iterator edge_iter;
    std::pair<edge_iter, edge_iter> ep;
    for (ep = boost::edges(baseGraph); ep.first != ep.second; ++ep.first) {
        boost::add_edge(boost::source(*ep.first, baseGraph), boost::target(*ep.first, baseGraph), scc3Graph);
    }

    // Graph initialization
    for (int i = 0; i < numVertices; i++) {
        boost::put(rIndex, i, 0);
        boost::put(isRoot, i, false);
    }

    index = 1;
    c = numVertices - 1;

}

void Pea_find_scc3::run() {
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
}

void Pea_find_scc3::visit(int v) {
    beginVisiting(v);

    while (!vS.isEmptyFront()) {
        visitLoop();
    }
}

void Pea_find_scc3::visitLoop() {
    int v = vS.topFront();
    int i = iS.topFront();

    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);

    typedef boost::graph_traits<SCC3Graph> GraphTraits;
    typename GraphTraits::out_edge_iterator out_i, out_end;
    typename GraphTraits::edge_descriptor e;

    // Count out-endges
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
}

void Pea_find_scc3::beginVisiting(int v) {
    // First time this node encountered
    vS.pushFront(v);
    iS.pushFront(0);

    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);
    isRoot[v] = true;
    rIndex[v] = index;
    index = index + 1;
}

void Pea_find_scc3::finishVisiting(int v) {
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
}

bool Pea_find_scc3::beginEdge(int v, int k) {
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);

    unsigned int w = getTargetVertex(v, k);
    if (rIndex[w] == 0) {
        iS.popFront();
        iS.pushFront(k + 1);
        beginVisiting(w);
        return true;
    } else {
        return false;
    }

}

void Pea_find_scc3::finishEdge(int v, int k) {
    boost::property_map<SCC3Graph, vertex_rIndex_t>::type rIndex = boost::get(vertex_rIndex, scc3Graph);
    boost::property_map<SCC3Graph, vertex_isRoot_t>::type isRoot = boost::get(vertex_isRoot, scc3Graph);

    int w = getTargetVertex(v, k);

    if (rIndex[w] < rIndex[v]) {
        rIndex[v] = rIndex[w];
        isRoot[v] = false;
    }

}

unsigned int Pea_find_scc3::getTargetVertex(int v, int k) {
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

    return w;
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