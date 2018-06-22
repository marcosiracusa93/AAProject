//
// Created by Andrea Pace on 08/06/18.
//

#ifndef AAPROJECT_TARJ_FIND_SCC1_HPP
#define AAPROJECT_TARJ_FIND_SCC1_HPP


#include <boost/pending/property.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <stack>

#include "../typedefs.hpp"

enum vertex_lowPt_t {
    vertex_lowPt
};
enum vertex_lowVine_t {
    vertex_lowVine
};
enum vertex_inComponent_t {
    vertex_inComponent
};

enum vertex_number_t {
    vertex_number
};

namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, lowPt);
    BOOST_INSTALL_PROPERTY(vertex, lowVine);
    BOOST_INSTALL_PROPERTY(vertex, number);
    BOOST_INSTALL_PROPERTY(vertex, inComponent); // not present in Tarjan's pseudocode but necessary in order to keep
                                                 // track of each node's ancestors
}

typedef boost::property<vertex_number_t, unsigned long> Number;
typedef boost::property<vertex_lowPt_t, unsigned long, Number> LowPt;
typedef boost::property<vertex_lowVine_t, unsigned long, LowPt> LowVine;
typedef boost::property<vertex_inComponent_t, bool, LowVine> InComponent;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, InComponent> SCC1Graph;


class Tarj_find_scc1 {

private:
    SCC1Graph scc1Graph;
    int index;
    std::stack<long> point_stack;

public:
    Tarj_find_scc1(const BaseGraph &baseGraph);

    void run();

    void getSCCResult(unsigned int *scc3);

private:
    void visit(long v);
};


#endif //AAPROJECT_TARJ_FIND_SCC1_HPP
