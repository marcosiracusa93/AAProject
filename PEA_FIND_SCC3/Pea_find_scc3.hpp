//
// Created by Marco Siracusa on 5/28/18.
//

#ifndef AAPROJECT_PEA_FIND_SCC3_HPP
#define AAPROJECT_PEA_FIND_SCC3_HPP


#include <boost/pending/property.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "../typedefs.hpp"
#include "../DoubleStack.hpp"

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
    DoubleStack vS;
    DoubleStack iS;
    int index;
    int c;

public:
    Pea_find_scc3(const BaseGraph &baseGraph);

    void run();

    void getSCCResult(unsigned int *scc3);

private:
    void visit(int v);

    void visitLoop();

    void beginVisiting(int v);

    void finishVisiting(int v);

    bool beginEdge(int v, int k);

    void finishEdge(int v, int k);

    unsigned int getTargetVertex(int v, int k);
};


#endif //AAPROJECT_PEA_FIND_SCC3_HPP
