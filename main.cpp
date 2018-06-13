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

    // Check input consistency
    if (argc != 4) {
        std::cout << "Wrong format. Expected parameters: " << std::endl;
        std::cout << " 1. number of vertices" << std::endl;
        std::cout << " 2. number of edges" << std::endl;
        std::cout << " 3. selected algorithm" << std::endl;
        exit(-1);
    }

    // Get num edges and vertices (which actually are global variables)
    g_numVertices = (unsigned int) strtol(argv[1], NULL, 10);
    g_numEdges = (unsigned int) strtol(argv[2], NULL, 10);

    // Get the id of the algorithm the user wants to run
    char algorithm = argv[3][0];

    // Create the graph
    BaseGraph graph(g_numVertices);

    // Randomize the graph
    graph.clear();
    boost::mt19937 rng;
    rng.seed(uint32_t(time(0)));
    boost::generate_random_graph(graph, g_numVertices, g_numEdges, rng, true, true);

    // Optionally print edges in the result_stream
    if (PRINT_EDGES) {
        result_stream << std::endl << "edges(g) = ";

        // Instantiate property map for accessing the index id
        boost::property_map<BaseGraph, boost::vertex_index_t>::type vertex_id = boost::get(boost::vertex_index, graph);
        // Instantiate an edge iterator
        boost::graph_traits<BaseGraph>::edge_iterator ei, ei_end;

        // Iterate through the edges and print
        for (boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei) {
            result_stream << "(" << get(vertex_id, source(*ei, graph))
                          << "," << get(vertex_id, target(*ei, graph)) << ") ";
        }

        result_stream << std::endl << std::endl;
    }

    // Select the algorithm according to the user input
    switch (algorithm) {
        case 'p': { /// Pearce's SCC algorithm

            result_stream << "*** pea_find_scc3: " << std::endl << std::endl;

            // Initialize algorithm's class
            Pea_find_scc3 pea_find_scc3 = Pea_find_scc3(graph);

            // Initialize timer
            boost::timer::cpu_timer timer;

            // Run the algorithm
            pea_find_scc3.run();

            // Stop timer
            boost::timer::cpu_times times = timer.elapsed();

            // Save wall time in the time_stream
            time_stream << times.wall << std::endl;

            // Print times in the result_stream
            result_stream << " Recorded times[ns]: " << std::endl;
            result_stream << "  Wall: " << times.wall << std::endl;
            result_stream << "  User: " << times.user << std::endl;
            result_stream << "  System: " << times.system << std::endl << std::endl;

            // Optionally print the result (just the time would be print otherwise)
            if (PRINT_RESULT) {
                // Allocate room for the result (a uint, the connected compoent, for each vertex)
                unsigned int *scc3_result = (unsigned int *) malloc(sizeof(unsigned int) * g_numVertices);

                // Get the result
                pea_find_scc3.getSCCResult(scc3_result);
                result_stream << " Output: " << std::endl;
                for (int i = 0; i < g_numVertices; i++) {
                    result_stream << "  " << i << " is in cc" << scc3_result[i] << std::endl;
                }

                // Free memory allocated for results
                free(scc3_result);
            }
        }
            break;

        default:
            std::cout << "Wrong algorithm" << std::endl;
            exit(-1);
    }

    // Select the stream according on what the user wants to print:
    //  - time_stream    -> time only (to be used with the script)
    //  - result_stream  -> wall, user and system time of the current run and, optionally, the algorithm's result
    if (PRINT_TIME_ONLY) {
        std::cout << time_stream.str();
    } else {
        std::cout << result_stream.str();
    }


    return 0;
}