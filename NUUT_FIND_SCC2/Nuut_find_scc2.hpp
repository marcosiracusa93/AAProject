//
// Created by Andrea Pace on 31/05/18.
//

#ifndef AAPROJECT_NUUTILA_SCC2_HPP
#define AAPROJECT_NUUTILA_SCC2_HPP


#include <boost/pending/property.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <stack>

#include "../utilities/typedefs.hpp"


enum vertex_scc_t {
    vertex_scc
};

enum vertex_vRoot_t {
    vertex_vRoot
};
enum vertex_inComponent_t {
    vertex_inComponent
};

namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, scc);
    BOOST_INSTALL_PROPERTY(vertex, vRoot);
    BOOST_INSTALL_PROPERTY(vertex, inComponent);
}

typedef boost::property<vertex_scc_t, unsigned int> VScc; // additional vector that orders vertices according to visit time
typedef boost::property<vertex_vRoot_t, long, VScc> VRoot;
typedef boost::property<vertex_inComponent_t, bool, VRoot> InComponent;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, InComponent> SCC2Graph;

class Nuut_find_scc2 {

private:
    SCC2Graph scc2Graph;
    std::stack<long> candidate_stack;  //Stack for final candidates root nodes
    unsigned int visit_index;


public:
    Nuut_find_scc2(unsigned int g_numVertices);

    Nuut_find_scc2(const BaseGraph &baseGraph, unsigned int g_numVertices);

    void run();

    void getSCCResult(unsigned int *scc2);

private:
    void visit(long v);
};

#endif //AAPROJECT_NUUTILA_SCC2_HPP
