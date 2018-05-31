//
// Created by Marco Siracusa on 5/28/18.
//

#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "typedefs.hpp"
#include "PEA_FIND_SCC3/Pea_find_scc3.hpp"

extern unsigned int g_numVertices;
extern unsigned int g_numEdges;

int main(int argc, char **argv) {

    if (argc != 3) {
        std::cout << "Wrong format. Expected parameters: " << std::endl;
        std::cout << " 1. number of vertices" << std::endl;
        std::cout << " 2. number of edges" << std::endl;
    }
    g_numVertices = (unsigned int) strtol(argv[1], NULL, 10);
    g_numEdges = (unsigned int) strtol(argv[2], NULL, 10);

    // Graph creation
    BaseGraph graph(g_numVertices);

    // Graph randomization
    graph.clear();
    boost::mt19937 rng;
    rng.seed(uint32_t(time(0)));
    boost::generate_random_graph(graph, g_numVertices, g_numEdges, rng, true, true);

    if (PRINT_EDGES) {
        std::cout << std::endl;
        boost::property_map<BaseGraph, boost::vertex_index_t>::type vertex_id = boost::get(boost::vertex_index, graph);
        std::cout << "edges(g) = ";
        boost::graph_traits<BaseGraph>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei)
            std::cout << "(" << get(vertex_id, source(*ei, graph))
                      << "," << get(vertex_id, target(*ei, graph)) << ") ";
        std::cout << std::endl << std::endl;
    }

    /// Pearce's SCC algorithm
    {
        Pea_find_scc3 pea_find_scc3 = Pea_find_scc3(graph);
        pea_find_scc3.run();

        unsigned int *scc3 = (unsigned int *) malloc(sizeof(unsigned int) * g_numVertices);

        pea_find_scc3.getSCCResult(scc3);

        if (PRINT_RESULT) {
            std::cout << "pea_find_scc3: " << std::endl;
            for (int i = 0; i < g_numVertices; i++) {
                std::cout << i << " is in cc" << scc3[i] << std::endl;
            }
        }
    }

    return 0;
}