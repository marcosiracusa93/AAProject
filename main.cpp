//
// Created by Marco Siracusa on 5/28/18.
//

#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/timer/timer.hpp>

#include "typedefs.hpp"
#include "PEA_FIND_SCC3/Pea_find_scc3.hpp"

extern unsigned int g_numVertices;
extern unsigned int g_numEdges;

int main(int argc, char **argv) {

    std::stringstream result_stream;
    std::stringstream time_stream;

    if (argc != 4) {
        std::cout << "Wrong format. Expected parameters: " << std::endl;
        std::cout << " 1. number of vertices" << std::endl;
        std::cout << " 2. number of edges" << std::endl;
        std::cout << " 3. selected algorithm" << std::endl;
        exit(-1);
    }
    g_numVertices = (unsigned int) strtol(argv[1], NULL, 10);
    g_numEdges = (unsigned int) strtol(argv[2], NULL, 10);

    char algorithm = (char) argv[3][0];

    // Graph creation
    BaseGraph graph(g_numVertices);

    // Graph randomization
    graph.clear();
    boost::mt19937 rng;
    rng.seed(uint32_t(time(0)));
    boost::generate_random_graph(graph, g_numVertices, g_numEdges, rng, true, true);

    if (PRINT_EDGES) {
        result_stream << std::endl;
        boost::property_map<BaseGraph, boost::vertex_index_t>::type vertex_id = boost::get(boost::vertex_index, graph);
        result_stream << "edges(g) = ";
        boost::graph_traits<BaseGraph>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei)
            result_stream << "(" << get(vertex_id, source(*ei, graph))
                          << "," << get(vertex_id, target(*ei, graph)) << ") ";
        result_stream << std::endl << std::endl;
    }

    /// Pearce's SCC algorithm
    switch (algorithm) {
        case 'p': {
            result_stream << "*** pea_find_scc3: " << std::endl << std::endl;
            Pea_find_scc3 pea_find_scc3 = Pea_find_scc3(graph);

            boost::timer::cpu_timer timer;

            pea_find_scc3.run();

            boost::timer::cpu_times times = timer.elapsed();

            time_stream << times.wall << std::endl;

            result_stream << " Recorded times[ns]: " << std::endl;
            result_stream << "  Wall: " << times.wall << std::endl;
            result_stream << "  User: " << times.user << std::endl;
            result_stream << "  System: " << times.system << std::endl << std::endl;

            unsigned int *scc3 = (unsigned int *) malloc(sizeof(unsigned int) * g_numVertices);

            pea_find_scc3.getSCCResult(scc3);

            if (PRINT_RESULT) {
                result_stream << " Output: " << std::endl;
                for (int i = 0; i < g_numVertices; i++) {
                    result_stream << "  " << i << " is in cc" << scc3[i] << std::endl;
                }
            }
        }
            break;
        default:
            std::cout << "Wrong algorithm" << std::endl;
            exit(-1);
    }


    if (PRINT_TIME_ONLY) {
        std::cout << time_stream.str();
    } else {
        std::cout << result_stream.str();
    }


    return 0;
}