//
// Created by Marco Siracusa on 5/28/18.
//

#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/timer/timer.hpp>
#include <boost/graph/graphml.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <Pea_find_scc3_old.hpp>

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

    BaseGraph graph;

    if (g_path != "random") {
        // Read the graph from .xml file
        std::ifstream inputGraph;
        inputGraph.open(g_path, std::ifstream::in);
        boost::dynamic_properties dp;
        boost::read_graphml(inputGraph, graph, dp);
    } else {
        // Randomize the graph
        graph.clear();
        boost::mt19937 rng;
        rng.seed(uint32_t(time(0)));
        boost::generate_random_graph(graph, g_numVertices, g_numEdges, rng, false, true);
    }

    unsigned int *scc_result_Pear = (unsigned int *) malloc(sizeof(unsigned int) * g_numVertices);
    unsigned int *scc_result_Pear_old = (unsigned int *) malloc(sizeof(unsigned int) * g_numVertices);
    unsigned int *scc_result_Nuut = (unsigned int *) malloc(sizeof(unsigned int) * g_numVertices);;
    unsigned int *scc_result_Tarj = (unsigned int *) malloc(sizeof(unsigned int) * g_numVertices);;

    Pea_find_scc3 scc_algorithm_Pear = Pea_find_scc3(graph, g_numVertices, g_numEdges);
    Pea_find_scc3_old scc_algorithm_Pear_old = Pea_find_scc3_old(graph, g_numVertices, g_numEdges);
    Nuut_find_scc2 scc_algorithm_Nuut = Nuut_find_scc2(graph, g_numVertices);
    Tarj_find_scc1 scc_algorithm_Tarj = Tarj_find_scc1(graph, g_numVertices);

    scc_algorithm_Pear.run();
    scc_algorithm_Pear.getSCCResult(scc_result_Pear);

    scc_algorithm_Pear_old.run();
    scc_algorithm_Pear_old.getSCCResult(scc_result_Pear_old);

    scc_algorithm_Nuut.run();
    scc_algorithm_Nuut.getSCCResult(scc_result_Nuut);

    scc_algorithm_Tarj.run();
    scc_algorithm_Tarj.getSCCResult(scc_result_Tarj);

    bool correct = true;
    for (int v1 = 0; v1 < g_numVertices; v1++) {

        std::vector<int> pearScc = std::vector<int>();
        for (int v2 = v1; v2 < g_numVertices - v1; v2++) {
            if (scc_result_Pear[v1] == scc_result_Pear[v2]) {
                pearScc.push_back(v2);
            }
        }

        std::vector<int> pearSccOld = std::vector<int>();
        for (int v2 = v1; v2 < g_numVertices - v1; v2++) {
            if (scc_result_Pear_old[v1] == scc_result_Pear_old[v2]) {
                pearSccOld.push_back(v2);
            }
        }

        std::vector<int> tarjScc = std::vector<int>();
        for (int v2 = v1; v2 < g_numVertices - v1; v2++) {
            if (scc_result_Tarj[v1] == scc_result_Tarj[v2]) {
                tarjScc.push_back(v2);
            }
        }

        std::vector<int> nuutScc = std::vector<int>();
        for (int v2 = v1; v2 < g_numVertices - v1; v2++) {
            if (scc_result_Nuut[v1] == scc_result_Nuut[v2]) {
                nuutScc.push_back(v2);
            }
        }

        if (pearScc.size() != pearSccOld.size() or pearSccOld.size() != tarjScc.size() or
            tarjScc.size() != nuutScc.size()) {
            correct = false;
            break;
        }

        for (int i = 0; i < pearScc.size(); i++) {
            if (pearScc[i] != pearSccOld[i] or pearSccOld[i] != tarjScc[i] or tarjScc[i] != nuutScc[i]) {
                correct = false;
                break;
            }
            if (!correct) {
                break;
            }
        }
    }

    std::cout << "Correctness: " << correct << std::endl << std::endl;

    if (PRINT_RESULT) {
        for (int i = 0; i < g_numVertices; i++) {
            std::cout << "O:  " << i << " is in cc" << scc_result_Pear_old[i] << std::endl;
        }

        std::cout << std::endl;

        for (int i = 0; i < g_numVertices; i++) {
            std::cout << "P:  " << i << " is in cc" << scc_result_Pear[i] << std::endl;
        }

        std::cout << std::endl;

        for (int i = 0; i < g_numVertices; i++) {
            std::cout << "N:  " << i << " is in cc" << scc_result_Nuut[i] << std::endl;
        }

        std::cout << std::endl;

        for (int i = 0; i < g_numVertices; i++) {
            std::cout << "T:  " << i << " is in cc" << scc_result_Tarj[i] << std::endl;
        }
    }

    free(scc_result_Pear);
    free(scc_result_Nuut);
    free(scc_result_Tarj);

    return correct;
}