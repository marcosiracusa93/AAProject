//
// Created by Marco Siracusa on 7/23/18.
//

#include "iostream"
#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/graph/graphml.hpp>
#include "typedefs.hpp"

void write_graph(BaseGraph g, std::string path) {
    std::ofstream out;
    boost::dynamic_properties dp;
    out.open(path);
    boost::write_graphml(out, g, dp);
    out.close();
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cout << "Wrong format. Expected parameters: " << std::endl;
        std::cout << " 1. number of vertices" << std::endl;
        std::cout << " 2. number of edges" << std::endl;
        std::cout << " 3. graph file path" << std::endl;
        exit(-1);
    }

    // Get num edges and vertices (which actually are global variables)
    unsigned int g_numVertices = (unsigned int) strtol(argv[1], NULL, 10);
    unsigned int g_numEdges = (unsigned int) strtol(argv[2], NULL, 10);


    // Get the id of the algorithm the user wants to run
    std::string path = argv[3];

    // Create the graph
    BaseGraph graph(g_numVertices);

    // Randomize the graph
    graph.clear();
    boost::mt19937 rng;
    rng.seed(uint32_t(time(0)));
    boost::generate_random_graph(graph, g_numVertices, g_numEdges, rng, false, true);

    write_graph(graph, path);
    return 0;
}



