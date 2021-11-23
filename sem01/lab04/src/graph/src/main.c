#include <stdio.h>
#include <stdbool.h>

#include "menu.h"
#include "graph.h"

#define OK 0

int main(int argc, char **argv)
{
    printf("\nПОИСК НЕДОСТИЖИМЫХ ВЕРШИН В ОРИЕНТИРОВАННОМ ГРАФЕ\n\n");

    char *inFileName = argv[1];
    FILE *inFile = fopen(inFileName, "r");

    graph_t graph = { .graph = NULL, .nodes = 0 }; 

    read_graph_from_stream(inFile, &graph);

    printf("Вершина для поиска недостижимых вершин: %u\n", 1);
    find_nodes(&graph, 1);

    return OK;
}
