//
// Created by Marco Siracusa on 5/28/18.
//

#ifndef AAPROJECT_PEA_FIND_SCC3_HPP
#define AAPROJECT_PEA_FIND_SCC3_HPP


#include <boost/pending/property.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "../utilities/typedefs.hpp"
#include "../utilities/DoubleStackVec.hpp"

enum vertex_rIndex_t {
    vertex_rIndex
};
enum vertex_isRoot_t {
    vertex_isRoot
};

namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, rIndex);
    BOOST_INSTALL_PROPERTY(vertex, isRoot);
}

typedef boost::property<vertex_isRoot_t, bool> IsRoot;
typedef boost::property<vertex_rIndex_t, unsigned int, IsRoot> RIndex;


// Creating the (directed) graph with the "rIndex" and "isRoot"
// properties attached to vertices
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, RIndex> SCC3Graph;


class Pea_find_scc3 {

private:
    SCC3Graph scc3Graph;
    DoubleStackVec<Vertex> vS;
    DoubleStackVec<int> iS;
    int index;
    int c;
    unsigned long long stack_dimension;


public:
    Pea_find_scc3(unsigned int g_numVertices, unsigned int g_numEdges);

    void run();

    void getSCCResult(unsigned int *scc3);

    unsigned long long getStackDimension();

private:
    void visit(Vertex v);

    void visitLoop();

    void beginVisiting(Vertex v);

    void finishVisiting(Vertex v);

    bool beginEdge(Vertex v, int k, boost::graph_traits<BaseGraph>::out_edge_iterator it);

    void finishEdge(Vertex v, int k, boost::graph_traits<BaseGraph>::out_edge_iterator it);
};


#endif //AAPROJECT_PEA_FIND_SCC3_HPP
