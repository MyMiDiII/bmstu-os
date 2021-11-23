#ifndef __GRAPH_H__

#define __GRAPH_H__

#include <stddef.h>
#include <io.h>

typedef struct node node_t;

struct node
{
    uint num;
    node_t *next_neighbour;
};

typedef struct edge
{
    uint from;
    uint to;
} edge_t;

typedef struct graph
{
    node_t **graph;
    uint nodes;
} graph_t;

int read_graph(graph_t *graph);

void free_graph(graph_t *graph);

int find_by_node(graph_t *graph);

int graphviz(graph_t *graph);

int read_graph_from_stream(FILE *stream, graph_t *graph);

int find_nodes(graph_t *graph, const uint node);

#endif
