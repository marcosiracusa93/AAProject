//
// Created by Andrea Pace on 20/06/18.
//

#include <iostream>
#include "Tarj_find_scc1.hpp"

Tarj_find_scc1::Tarj_find_scc1(const BaseGraph &baseGraph, unsigned int g_numVertices) : scc1Graph(g_numVertices) {

    boost::property_map<SCC1Graph, vertex_inStack_t >::type inStack = get(vertex_inStack, scc1Graph);
    boost::property_map<SCC1Graph, vertex_lowPt_t >::type lowPt = get(vertex_lowPt, scc1Graph);
    boost::property_map<SCC1Graph, vertex_lowVine_t >::type lowVine = get(vertex_lowVine, scc1Graph);
    boost::property_map<SCC1Graph, vertex_number_t >::type number = get(vertex_number, scc1Graph);

    typedef boost::graph_traits<BaseGraph>::edge_iterator edge_iter;
    std::pair<edge_iter, edge_iter> ep;

    // Building the graph
    for (ep = boost::edges(baseGraph); ep.first != ep.second; ++ep.first) {
        boost::add_edge(boost::source(*ep.first, baseGraph), boost::target(*ep.first, baseGraph), scc1Graph);
    }

    // Graph properties initialization
    for (int i = 0; i < g_numVertices; i++) {
        boost::put(lowPt, i,0);
        boost::put(inStack, i, false);
        boost::put(lowVine, i, 0);
        boost::put(number, i, 0);
    }

    index=0;

    // Stack initialization
    point_stack.push(-1);   // each node is pushed on the stack at the beginning of visit()
    ancestor = (bool *) malloc(sizeof(bool) * g_numVertices);
}

void Tarj_find_scc1::run() {
    typedef boost::graph_traits<SCC1Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    boost::property_map<SCC1Graph, vertex_number_t>::type number = get(vertex_number, scc1Graph);

    for (vp = boost::vertices(scc1Graph); vp.first != vp.second; ++vp.first) {
        // Call visit() on non-visited vertices
        if (number[*vp.first] == 0) {
            visit(*vp.first);
        }
    }
}

void Tarj_find_scc1::visit(long v) {
    boost::property_map<SCC1Graph, vertex_number_t>::type number = get(vertex_number, scc1Graph);
    boost::property_map<SCC1Graph, vertex_lowPt_t >::type lowPt = get(vertex_lowPt, scc1Graph);
    boost::property_map<SCC1Graph, vertex_lowVine_t >::type lowVine = get(vertex_lowVine, scc1Graph);
    boost::property_map<SCC1Graph, vertex_inStack_t >::type inStack = get(vertex_inStack, scc1Graph);

    lowPt[v] = lowVine[v] = number[v] = ++index;

    point_stack.push(v);
    inStack[v] = true;

    typename boost::graph_traits<SCC1Graph>::out_edge_iterator out_i, out_end;
    long w;
    for (boost::tie(out_i, out_end) = out_edges(v, scc1Graph); out_i != out_end; ++out_i) {
        w = boost::target(*out_i,scc1Graph);

        if (number[w] == 0) {  // w not yet visited -> (v,w) is a tree arc
            ancestor[v] = true; // v is now the ancestor of every node in the visit trees traversing w
            visit(w);
            lowPt[v] = std::min(lowPt[v], lowPt[w]);
            lowVine[v] = std::min(lowVine[v], lowVine[w]);
        }

        else if (ancestor[w]) // w is an ancestor of v -> (v,w) is a frond
                lowPt[v] = std::min(lowPt[v], number[w]);

        else if (number[w] < number[v])  // (v,w) is a vine
                if (inStack[w])
                    lowVine[v] = std::min(lowVine[v], number[w]);
    }

    if (lowPt[v] == number[v] && lowVine[v] == number[v]) { // v is the root of a scc
        while (point_stack.top() != -1 && number[point_stack.top()] >= number[v]) {
            w = point_stack.top();
            point_stack.pop();
            inStack[w] = false;
            number[w] = number[v];     // not present in original algorithm, but it doesn't affect the code and
                                       // each node in the same component will eventually have the same number
        }
        inStack[v] = false;
    }
    ancestor[v] = false;
}

void Tarj_find_scc1::getSCCResult(unsigned int *scc1) {

    int i = 0;

    typedef boost::graph_traits<SCC1Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    boost::property_map<SCC1Graph, vertex_number_t>::type number = get(vertex_number, scc1Graph);
    for (vp = boost::vertices(scc1Graph), i = 0; vp.first != vp.second; ++vp.first, i++) {
        scc1[i] = boost::get(number, *vp.first);
    }
}



