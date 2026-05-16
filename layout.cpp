// Copyright 2004 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Douglas Gregor
//           Andrew Lumsdaine
#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topology.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <boost/random/linear_congruential.hpp>
#include <boost/timer/progress_display.hpp>
#include <boost/shared_ptr.hpp>

#include "layout.h"

using namespace boost;



void usage()
{
    std::cerr << "Usage: fr_layout [options] <width> <height>\n"
              << "Arguments:\n"
              << "\t<width>\tWidth of the display area (floating point)\n"
              << "\t<Height>\tHeight of the display area (floating point)\n\n"
              << "Options:\n"
              << "\t--iterations n\tNumber of iterations to execute.\n"
              << "\t\t\tThe default value is 100.\n"
              << "Input:\n"
              << "  Input is read from standard input as a list of edges, one "
                 "per line.\n"
              << "  Each edge contains two string labels (the endpoints) "
                 "separated by a space.\n\n"
              << "Output:\n"
              << "  Vertices and their positions are written to standard "
                 "output with the label,\n  x-position, and y-position of a "
                 "vertex on each line, separated by spaces.\n";
}

typedef boost::rectangle_topology<> topology_type;
typedef topology_type::point_type point_type;

typedef adjacency_list< listS, vecS, undirectedS,
    property< vertex_name_t, std::string > >
    Graph;

typedef graph_traits< Graph >::vertex_descriptor Vertex;

typedef std::map< std::string, Vertex > NameToVertex;

typedef std::vector< point_type > PositionVec;

typedef iterator_property_map< PositionVec::iterator,
			       property_map< Graph, vertex_index_t >::type >
PositionMap;


Vertex get_vertex(const std::string& name, Graph& g, NameToVertex& names)
{
    NameToVertex::iterator i = names.find(name);
    if (i == names.end())
        i = names.insert(std::make_pair(name, add_vertex(name, g))).first;
    return i->second;
}

class progress_cooling : public linear_cooling< double >
{
    typedef linear_cooling< double > inherited;

public:
  explicit progress_cooling(std::size_t iterations, layout_update_cb_t f ) : inherited(iterations)
    {
        display.reset(new boost::timer::progress_display(iterations + 1, std::cerr));
	iter=0;
	cb = f;
    }

    double operator()()
    {
        ++(*display);
	iter += 1;
	cb( iter );
        return inherited::operator()();
    }

private:
  shared_ptr< boost::timer::progress_display > display;
  int iter;
  layout_update_cb_t cb;
};


void layout_open( void ** graph, void ** labels ){
  Graph *g		= new Graph;
  NameToVertex *names	= new NameToVertex;
  *graph		= (void *) g;
  *labels		= (void *) names;
}

void layout_close( void *g, void *names, void *pos ){
  if( g )	delete (Graph *) g;
  if( names )	delete (NameToVertex *) names;
  if( pos )	delete (PositionMap *) pos;
}

void layout_add_edge( void *graph, void *labels, char *s, char *t ){
  Graph *g = (Graph *) graph;
  NameToVertex *names = (NameToVertex *) labels;
  std::string source(s), target(t);
  add_edge(get_vertex( source, *g, *names ),
	   get_vertex( target, *g, *names ), *g );
}

void layout_run( void *graph, void **pos, int iterations, double width, double height, layout_update_cb_t f ){
  Graph *g			= (Graph *) graph;
  PositionVec *position_vec	= new PositionVec(num_vertices(*g));
  PositionMap *position		= new PositionMap((*position_vec).begin(), get(vertex_index, *g));
  *pos = (void *) position;

  minstd_rand gen;
  topology_type topo(gen, -width / 2, -height / 2, width / 2, height / 2);
  random_graph_layout(*g, *position, topo);
  fruchterman_reingold_force_directed_layout( *g, *position, topo, cooling(progress_cooling(iterations, f)));
  //
  //
  // graph_traits< Graph >::vertex_iterator vi, vi_end;
  // for (boost::tie(vi, vi_end) = vertices( *g ); vi != vi_end; ++vi)
  //   {
  //     std::cout << get(vertex_name, *g, *vi) << '\t' << (*position)[*vi][0] << '\t' << typeid( (*position)[*vi][0] ).name() 
  // 		<< '\t' << (*position)[*vi][1] << std::endl;
  //   }
  
}

void layout_enumerate_vertices( void *graph, void *labels, void *pos, layout_enumv_cb_t f ){
  Graph *g = (Graph *) graph;
  PositionMap *position = (PositionMap *) pos;
  graph_traits< Graph >::vertex_iterator vi, vi_end;
  for (boost::tie(vi, vi_end) = vertices( *g ); vi != vi_end; ++vi)
    {
      f( (char *) get(vertex_name, *g, *vi).c_str(),  (*position)[*vi][0],  (*position)[*vi][1] );
      // std::cout << get(vertex_name, *g, *vi) << '\t' << (*position)[*vi][0]
      // 		<< '\t' << (*position)[*vi][1] << std::endl;
    }
}

void layout_enumerate_edges( void *graph, void *labels, void *pos, layout_enume_cb_t f ){
  Graph *g = (Graph *) graph;
  PositionMap *position = (PositionMap *) pos;
  graph_traits< Graph >::edge_iterator ei, ei_end;
  for (boost::tie(ei, ei_end) = edges( *g ); ei != ei_end; ++ei){
    f( (char *) get( vertex_name, *g, source( *ei, *g ) ).c_str(),
       (*position)[source( *ei, *g )][0],
       (*position)[source( *ei, *g )][1],
       (char *) get( vertex_name, *g, target( *ei, *g ) ).c_str(),
       (*position)[target( *ei, *g )][0],
       (*position)[target( *ei, *g )][1] );
  }
}

// int main(int argc, char* argv[])
// {
//     int iterations = 100;

//     if (argc < 3)
//     {
//         usage();
//         return -1;
//     }

//     double width = 0;
//     double height = 0;

//     for (int arg_idx = 1; arg_idx < argc; ++arg_idx)
//     {
//         std::string arg = argv[arg_idx];
//         if (arg == "--iterations")
//         {
//             ++arg_idx;
//             if (arg_idx >= argc)
//             {
//                 usage();
//                 return -1;
//             }
//             iterations = lexical_cast< int >(argv[arg_idx]);
//         }
//         else
//         {
//             if (width == 0.0)
//                 width = lexical_cast< double >(arg);
//             else if (height == 0.0)
//                 height = lexical_cast< double >(arg);
//             else
//             {
//                 usage();
//                 return -1;
//             }
//         }
//     }

//     if (width == 0.0 || height == 0.0)
//     {
//         usage();
//         return -1;
//     }

//     Graph g;
//     NameToVertex names;

//     std::string source, target;
//     while (std::cin >> source >> target)
//     {
//         add_edge(get_vertex(source, g, names), get_vertex(target, g, names), g);
//     }

//     typedef std::vector< point_type > PositionVec;
//     PositionVec position_vec(num_vertices(g));
//     typedef iterator_property_map< PositionVec::iterator,
//         property_map< Graph, vertex_index_t >::type >
//         PositionMap;
//     PositionMap position(position_vec.begin(), get(vertex_index, g));

//     minstd_rand gen;
//     topology_type topo(gen, -width / 2, -height / 2, width / 2, height / 2);
//     random_graph_layout(g, position, topo);
//     fruchterman_reingold_force_directed_layout(
//         g, position, topo, cooling(progress_cooling(iterations)));

//     graph_traits< Graph >::vertex_iterator vi, vi_end;
//     for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
//     {
//         std::cout << get(vertex_name, g, *vi) << '\t' << position[*vi][0]
//                   << '\t' << position[*vi][1] << std::endl;
//     }
//     return 0;
// }
