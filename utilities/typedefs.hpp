//
// Created by Marco Siracusa on 5/28/18.
//

#ifndef AAPROJECT_TYPEDEFS_HPP
#define AAPROJECT_TYPEDEFS_HPP

#define PRINT_EDGES false
#define PRINT_RESULT false
#define PRINT_VERBOSE false
#define PRINT_MEASUREMENTS true
#define TRACK_STACK_CONSUMPTION true

extern unsigned int g_numVertices;
extern unsigned int g_numEdges;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> BaseGraph;

#endif //AAPROJECT_TYPEDEFS_HPP
