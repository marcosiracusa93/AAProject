//
// Created by Marco Siracusa on 5/28/18.
//

#include "Pea_find_scc3.hpp"


Pea_find_scc3::Pea_find_scc3(const BaseGraph &baseGraph) : scc3Graph(numVertices) {
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
}