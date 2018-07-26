//
// Created by Marco Siracusa on 7/26/18.
//


#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/timer/timer.hpp>
#include <boost/graph/graphml.hpp>

#include <boost/property_tree/json_parser.hpp>

#include "../utilities/typedefs.hpp"
#include "Pea_find_scc3.hpp"

unsigned int g_numVertices;
unsigned int g_numEdges;
std::string g_path;

int main(int argc, char **argv) {

    std::stringstream measurements_stream;

    // Check input consistency
    if (argc != 4) {
        std::cout << "Wrong format. Expected parameters: " << std::endl;
        std::cout << " 1. number of vertices" << std::endl;
        std::cout << " 2. number of edges" << std::endl;
        std::cout << " 3. file path" << std::endl;
        exit(-1);
    }

    // Get num edges and vertices (which actually are global variables)
    g_numVertices = (unsigned int) strtol(argv[1], NULL, 10);
    g_numEdges = (unsigned int) strtol(argv[2], NULL, 10);
    g_path = argv[3];

    // Read the graph from .xml file
    std::ifstream inputGraph;
    BaseGraph graph;
    inputGraph.open(g_path, std::ifstream::in);
    boost::dynamic_properties dp;
    boost::read_graphml(inputGraph, graph, dp);

    unsigned long long stack_dimension = 0;
    boost::timer::cpu_times times;
    times.clear();

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

    boost::property_tree::ptree root;
    root.put("elapsed_time", times.wall);
    root.put("stack_dimension", stack_dimension);

    boost::property_tree::write_json(measurements_stream, root);

    std::cout << measurements_stream.str();

    return 0;
}