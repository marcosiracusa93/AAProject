//
// Created by Marco Siracusa on 5/28/18.
//

#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/timer/timer.hpp>
#include <boost/graph/graphml.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "utilities/typedefs.hpp"
#include "PEA_FIND_SCC3/Pea_find_scc3.hpp"
#include "TARJ_FIND_SCC1/Tarj_find_scc1.hpp"
#include "NUUT_FIND_SCC2/Nuut_find_scc2.hpp"

unsigned int g_numVertices;
unsigned int g_numEdges;
std::string g_path;

int main(int argc, char **argv) {

    std::stringstream verbose_stream;
    std::stringstream measurements_stream;

    // Check input consistency
    if (argc != 5) {
        std::cout << "Wrong format. Expected parameters: " << std::endl;
        std::cout << " 1. number of vertices" << std::endl;
        std::cout << " 2. number of edges" << std::endl;
        std::cout << " 3. file path" << std::endl;
        std::cout << " 4. selected algorithm" << std::endl;
        exit(-1);
    }

    // Get num edges and vertices (which actually are global variables)
    g_numVertices = (unsigned int) strtol(argv[1], NULL, 10);
    g_numEdges = (unsigned int) strtol(argv[2], NULL, 10);
    g_path = argv[3];

    // Get the id of the algorithm the user wants to run
    char algorithm = argv[4][0];

    // Read the graph from .xml file
    std::ifstream inputGraph;
    BaseGraph graph;
    inputGraph.open(g_path, std::ifstream::in);
    boost::dynamic_properties dp;
    boost::read_graphml(inputGraph, graph, dp);

    // Optionally print edges in the verbose_stream
    if (PRINT_EDGES) {
        verbose_stream << std::endl << "edges(g) = ";

        // Instantiate property map for accessing the index id
        boost::property_map<BaseGraph, boost::vertex_index_t>::type vertex_id = boost::get(boost::vertex_index, graph);
        // Instantiate an edge iterator
        boost::graph_traits<BaseGraph>::edge_iterator ei, ei_end;

        // Iterate through the edges and print
        for (boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei) {
            verbose_stream << "(" << get(vertex_id, source(*ei, graph))
                           << "," << get(vertex_id, target(*ei, graph)) << ") ";
        }

        verbose_stream << std::endl << std::endl;
    }

    std::string algorithm_name;

    unsigned int *scc_result = nullptr;

    if (PRINT_RESULT) {
        // Allocate room for the result (a uint, the connected component, for each vertex)
        scc_result = (unsigned int *) malloc(sizeof(unsigned int) * g_numVertices);
    }

    unsigned long long stack_dimension = 0;
    boost::timer::cpu_times times;
    times.clear();

    // Select the algorithm according to the user input
    switch (algorithm) {
        case 'p': { /// Pearce's SCC algorithm

            algorithm_name = "Pearce";

            // Initialize algorithm's class
            Pea_find_scc3 scc_algorithm = Pea_find_scc3(graph, g_numVertices, g_numEdges);

            // Initialize timer
            boost::timer::cpu_timer timer;

            // Run the algorithm
            scc_algorithm.run();

            // Stop timer
            times = timer.elapsed();

            // Get stack dimension
            stack_dimension = scc_algorithm.getStackDimension();

            if (PRINT_RESULT) {
                // Get result
                scc_algorithm.getSCCResult(scc_result);
            }
            break;
        }

        case 'n': { /// Nuutila's Scc algorithm

            algorithm_name = "Nuutila";

            Nuut_find_scc2 scc_algorithm = Nuut_find_scc2(graph, g_numVertices);

            boost::timer::cpu_timer timer;

            scc_algorithm.run();

            times = timer.elapsed();

            if (PRINT_RESULT) {
                // Get result
                scc_algorithm.getSCCResult(scc_result);
            }
            break;
        }

        case 't': { /// Tarjan's Scc algorithm

            algorithm_name = "Tarjan";

            Tarj_find_scc1 scc_algorithm = Tarj_find_scc1(graph, g_numVertices);

            boost::timer::cpu_timer timer;

            scc_algorithm.run();

            times = timer.elapsed();

            if (PRINT_RESULT) {
                // Get result
                scc_algorithm.getSCCResult(scc_result);
            }
            break;
        }

        default:
            std::cout << "Wrong algorithm" << std::endl;
            exit(-1);
    }

    if (PRINT_VERBOSE && PRINT_MEASUREMENTS) {
        std::cout << "Cannot print both whole data and measurements" << std::endl;
        exit(-1);
    }

    // Select the stream according on what the user wants to print:
    //  - measurements_stream    -> time and stack consumption (to be used with the script)
    //  - verbose_stream  -> wall, user and system time of the current run, stack consumption and, optionally, the algorithm's result


    if (PRINT_MEASUREMENTS) {
        boost::property_tree::ptree root;
        root.put("elapsed_time", times.wall);
        root.put("stack_dimension", stack_dimension);

        std::stringstream sstream;

        boost::property_tree::write_json(sstream, root);

        // Save wall time and stack consumption in the measurements_stream
        measurements_stream << sstream.str() << std::endl;

        std::cout << measurements_stream.str();
    }

    if (PRINT_VERBOSE) {
        verbose_stream << "*** " << algorithm_name << "_find_scc: " << std::endl << std::endl;

        // Print times in the verbose_stream
        verbose_stream << " Recorded times[ns]: " << std::endl;
        verbose_stream << "  Wall: " << times.wall << std::endl;
        verbose_stream << "  User: " << times.user << std::endl;
        verbose_stream << "  System: " << times.system << std::endl << std::endl;

        verbose_stream << " Stack consumption: " << stack_dimension << std::endl << std::endl;

        // Optionally print the result
        if (PRINT_RESULT) {
            for (int i = 0; i < g_numVertices; i++)
                verbose_stream << "  " << i << " is in cc" << scc_result[i] << std::endl;

            // Free memory allocated for results
            free(scc_result);
        }

        std::cout << verbose_stream.str();
    }

    return 0;
}