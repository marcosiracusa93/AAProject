//
// Created by Andrea Pace on 08/06/18.
//

#ifndef AAPROJECT_TARJ_FIND_SCC1_HPP
#define AAPROJECT_TARJ_FIND_SCC1_HPP


#include <boost/pending/property.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <stack>

#include "../utilities/typedefs.hpp"

enum vertex_lowPt_t {
    vertex_lowPt
};
enum vertex_lowVine_t {
    vertex_lowVine
};
enum vertex_inStack_t {
    vertex_inStack
};
enum vertex_ancestor_t {
    vertex_ancestor
};

enum vertex_number_t {
    vertex_number
};

namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, lowPt);
    BOOST_INSTALL_PROPERTY(vertex, lowVine);
    BOOST_INSTALL_PROPERTY(vertex, number);
    BOOST_INSTALL_PROPERTY(vertex, inStack);  // tracks if each node is on the stack
}

typedef boost::property<vertex_number_t, unsigned long> Number;
typedef boost::property<vertex_lowPt_t, unsigned long, Number> LowPt;
typedef boost::property<vertex_lowVine_t, unsigned long, LowPt> LowVine;
typedef boost::property<vertex_inStack_t, bool, LowVine> InStack;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, InStack> SCC1Graph;


class Tarj_find_scc1 {

private:
    SCC1Graph scc1Graph;
    unsigned long index;
    std::stack<long> point_stack;
    bool *ancestor; // ancestor[w] is true iff w is the ancestor of the currently visited node

public:
    Tarj_find_scc1(const BaseGraph &baseGraph);

    void run();

    void getSCCResult(unsigned int *scc3);

private:
    void visit(long v);
};


#endif //AAPROJECT_TARJ_FIND_SCC1_HPP
