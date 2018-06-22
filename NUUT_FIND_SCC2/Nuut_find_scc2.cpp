//
// Created by Andrea Pace on 31/05/18.
//

#include <iostream>
#include "Nuut_find_scc2.hpp"

unsigned int g_numVertices;
unsigned int g_numEdges;

Nuut_find_scc2::Nuut_find_scc2(const BaseGraph &baseGraph) : scc2Graph(g_numVertices) {

    boost::property_map<SCC2Graph, vertex_inComponent_t >::type inComponent = get(vertex_inComponent, scc2Graph);
    boost::property_map<SCC2Graph, vertex_vRoot_t >::type root = get(vertex_vRoot, scc2Graph);
    boost::property_map<SCC2Graph, vertex_scc_t>::type scc = get(vertex_scc, scc2Graph);

    typedef boost::graph_traits<BaseGraph>::edge_iterator edge_iter;
    std::pair<edge_iter, edge_iter> ep;

    // Building the graph
    for (ep = boost::edges(baseGraph); ep.first != ep.second; ++ep.first) {
        boost::add_edge(boost::source(*ep.first, baseGraph), boost::target(*ep.first, baseGraph), scc2Graph);
    }

    // Graph properties initialization
    for (int i = 0; i < g_numVertices; i++) {
        boost::put(root, i,-1);
        boost::put(inComponent, i, false);
        boost::put(scc, i, 0);
    }
    visit_index=0;

    // Stack initialization
    candidate_stack.push(-1);
}

void Nuut_find_scc2::run() {
    typedef boost::graph_traits<SCC2Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    boost::property_map<SCC2Graph, vertex_vRoot_t >::type root = get(vertex_vRoot, scc2Graph);
    boost::property_map<SCC2Graph, vertex_scc_t>::type scc = get(vertex_scc, scc2Graph);

    for (vp = boost::vertices(scc2Graph); vp.first != vp.second; ++vp.first) {
        // Call visit() on non-visited vertices
        if (scc[*vp.first] == 0) {
            visit(*vp.first);
        }
    }
}

void Nuut_find_scc2::visit(long v) {
    boost::property_map<SCC2Graph, vertex_vRoot_t >::type root = get(vertex_vRoot, scc2Graph);
    boost::property_map<SCC2Graph, vertex_inComponent_t >::type inComponent = get(vertex_inComponent, scc2Graph);
    boost::property_map<SCC2Graph, vertex_scc_t>::type scc = get(vertex_scc, scc2Graph);

    root[v] = v;
    inComponent[v] = false;
    scc[v] = ++visit_index;

    typename boost::graph_traits<SCC2Graph>::out_edge_iterator out_i, out_end;
    long w;
    for (boost::tie(out_i, out_end) = out_edges(v, scc2Graph); out_i != out_end; ++out_i) {
            w = boost::target(*out_i,scc2Graph);

            // Depth-first visit of v's neighbors if not already visited
            if (scc[w] == 0) {
                visit(w);
            }
            if (!inComponent[w]) {
                if (scc[root[w]] < scc[root[v]]) {  // replaces root[w] < root[v] in Nuutila's paper
                    root[v] = root[w];
                }
            }
    }

    // Identify scc having v as its root
    if (root[v] == v) {
        inComponent[v] = true;
        while(candidate_stack.top() != -1 && scc[candidate_stack.top()] > scc[v]) {
            w = candidate_stack.top();
            candidate_stack.pop();
            scc[w] = scc[v];   // additional assignment which correctly assigns v's index to every w in the same scc
            inComponent[w] = true;
        }
    } else candidate_stack.push(v);
}

void Nuut_find_scc2::getSCCResult(unsigned int *scc2) {

    int i = 0;

    typedef boost::graph_traits<SCC2Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    boost::property_map<SCC2Graph, vertex_scc_t>::type scc = get(vertex_scc, scc2Graph);
    boost::property_map<SCC2Graph, vertex_vRoot_t >::type root = get(vertex_vRoot, scc2Graph);
    for (vp = boost::vertices(scc2Graph), i = 0; vp.first != vp.second; ++vp.first, i++) {
        scc2[i] = boost::get(scc, *vp.first);
    }
}