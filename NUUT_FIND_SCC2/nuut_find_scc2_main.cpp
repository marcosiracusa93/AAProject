//
// Created by Marco Siracusa on 7/26/18.
//


#include <boost/graph/random.hpp>
#include <boost/timer/timer.hpp>
#include <boost/graph/graphml.hpp>

#include <boost/property_tree/json_parser.hpp>

#include "../utilities/typedefs.hpp"
#include "Nuut_find_scc2.hpp"

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

    unsigned long long stack_dimension = 0;
    boost::timer::cpu_times times;
    times.clear();

    Nuut_find_scc2 scc_algorithm = Nuut_find_scc2(g_numVertices);

    boost::timer::cpu_timer timer;

    scc_algorithm.run();

    times = timer.elapsed();

    boost::property_tree::ptree root;
    root.put("elapsed_time", times.wall);
    root.put("stack_dimension", stack_dimension);

    boost::property_tree::write_json(measurements_stream, root);

    std::cout << measurements_stream.str();

    return 0;
}