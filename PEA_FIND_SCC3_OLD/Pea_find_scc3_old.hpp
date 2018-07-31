//
// Created by Marco Siracusa on 5/28/18.
//

#ifndef AAPROJECT_PEA_FIND_SCC3_OLD_HPP
#define AAPROJECT_PEA_FIND_SCC3_OLD_HPP


#include <boost/pending/property.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "../utilities/typedefs.hpp"
#include "../utilities/DoubleStackVec.hpp"

#ifndef PEA_DEF
#define PEA_DEF

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

#endif //PEA_DEF

class Pea_find_scc3_old {

private:
    SCC3Graph scc3Graph;
    DoubleStackVec<int> vS;
    DoubleStackVec<int> iS;
    int index;
    int c;
    unsigned long long stack_dimension;


public:
    Pea_find_scc3_old(const BaseGraph &baseGraph, unsigned int g_numVertices, unsigned int g_numEdges);

    void run();

    void getSCCResult(unsigned int *scc3);

    unsigned long long getStackDimension();

private:
    void visit(int v);

    void visitLoop();

    void beginVisiting(int v);

    void finishVisiting(int v);

    bool beginEdge(int v, int k);

    void finishEdge(int v, int k);

    unsigned int getTargetVertex(int v, int k);
};


#endif //AAPROJECT_PEA_FIND_SCC3_OLD_HPP