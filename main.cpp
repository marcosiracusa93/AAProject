//
// Created by Marco Siracusa on 5/28/18.
//

#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "PEA_FIND_SCC3/Pea_find_scc3.hpp"

int main() {
    // Graph creation
    BaseGraph graph(numVertices);

    // Graph randomization
    graph.clear();
    boost::mt19937 rng;
    rng.seed(uint32_t(time(0)));
    boost::generate_random_graph(graph, numVertices, numEdges, rng, true, true);

    Pea_find_scc3 pea_find_scc3 = Pea_find_scc3(graph);

    return 0;
}