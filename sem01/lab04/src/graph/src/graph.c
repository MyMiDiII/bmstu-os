#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "io.h"
#include "err.h"
#include "queue.h"

int graph_init(graph_t *graph, const uint size)
{
    graph->nodes = size;
    graph->graph = malloc(sizeof(node_t *) * size);

    if (!graph->graph)
        return ERR_MEMORY;

    for (int i = 0; i < size; i++)
        graph->graph[i] = NULL;

    return OK;
}

int push(node_t **list, uint node)
{
    node_t *elem = malloc(sizeof(node_t));

    if (!elem)
        return ERR_MEMORY;

    elem->num = node;
    elem->next_neighbour = NULL;

    if (!*list)
        *list = elem;
    else
    {
        node_t *tmp = *list;

        while (tmp->next_neighbour)
        {
            if (node == tmp->num)
            {
                free(elem);
                return OK;
            }
            tmp = tmp->next_neighbour;
        }

        tmp->next_neighbour = elem;
    }

    return OK;
}

int read_edge(FILE *stream, edge_t *const edge, const uint nodes)
{
    int exit_code = OK;

    exit_code = read_uint_from_file(stream, &edge->from);

    if (exit_code)
    {
        if (stdin == stream)
            clear_stdin();
        return exit_code;
    }

    if (edge->from <= 0 || edge->from > nodes)
    {
        if (stdin == stream)
            clear_stdin();
        return ERR_EDGE;
    }

    exit_code = read_uint_from_file(stream, &edge->to);

    if (exit_code)
    {
        if (stdin == stream)
            clear_stdin();
        return exit_code;
    }

    if (edge->to <= 0 || edge->to > nodes)
    {
        if (stdin == stream)
            clear_stdin();
        return ERR_EDGE;
    }

    return OK;
}

int read_graph_from_stream(FILE *stream, graph_t *graph)
{
    if (!graph)
        return ERR_MEMORY;

    int exit_code = OK;
    uint nodes = 0;
    uint edges = 0;
    graph->graph = NULL;
    graph->nodes = 0;

    if (stdin == stream)
        puts("Введите количество вершин в графе:");

    puts("Заданный граф:");
    exit_code = read_uint_from_file(stream, &nodes);

    if (!nodes)
        exit_code = ERR_NONUINT;

    if (!exit_code)
        exit_code = graph_init(graph, nodes);

    printf("Количество вершин: %u\n", nodes);
    if (!exit_code && stdin == stream)
        puts("Введите количество ребер в графе:");

    if (!exit_code)
        exit_code = read_uint_from_file(stream, &edges);
    printf("Количество ребер: %u\n", edges);

    if (!exit_code)
    {
        if (stdin == stream)
            printf("Введите %u пар вершин \"из -- в\" (через пробел):\n", edges);

        printf("Ребра:\n");
        edge_t edge = { .from = 0, .to = 0 };
        for (uint i = 0; !exit_code && i < edges; i++)
        {
            exit_code = read_edge(stream, &edge, nodes);
            
            if (!exit_code)
                exit_code = push(&graph->graph[edge.from - 1], edge.to);
            printf("%u --> %u\n", edge.from, edge.to);
        }
    }

    if (stdin == stream)
        clear_stdin();

    return exit_code;
}

int read_graph(graph_t *graph)
{
    int exit_code = OK;

    puts("Откуда считать данные?");
    puts("1 - с клавиатуры");
    puts("2 - из файла");
    puts("0 - вернуться в меню");

    int answer;

    exit_code = ERR_NONANSWER;
    while (exit_code)
    {
        exit_code = read_answer(&answer);

        if (exit_code)
            puts("Такого пункта меня нет! Попробуйте ещё раз!");
    }

    char *filename;
    FILE *file;
    bool is_open = false;

    if (!exit_code)
    {
        switch (answer)
        {
            case (1):
                exit_code = read_graph_from_stream(stdin, graph);
                break;
            case (2):
                puts("Введите имя файла:");
                exit_code = read_string(&filename, stdin);

                if (!exit_code)
                {
                    exit_code = open_file(&file, filename, &is_open);
                    free(filename);
                }

                if (!exit_code)
                    exit_code = read_graph_from_stream(file, graph);

                if (is_open)
                    fclose(file);

                break;
            default:
                break;
        }
    }

    return exit_code; 
}

void free_list(node_t **head)
{
    if (!head)
        return;

    for (node_t *cur = *head; cur; )
    {
        node_t *next = cur->next_neighbour;
        free(cur);
        cur = next;
    }

    *head = NULL;
}

void free_graph(graph_t *graph)
{
    for (int i = 0; i < graph->nodes; i++)
    {
        free_list(&graph->graph[i]);
    }

    free(graph->graph);
}

int graphviz(graph_t *graph)
{
    if (!graph->nodes)
        return ERR_EMPTY_GRAPH;

    FILE *file = fopen("out/graph.gv", "w");

    if (!file )
        return ERR_OPEN_FILE;

    fprintf(file, "digraph {\n");

    for (uint i = 0; i < graph->nodes; i++)
    {
        if (!graph->graph[i])
            fprintf(file, "    %u;\n", i + 1);

        for (node_t *cur = graph->graph[i]; cur; cur = cur->next_neighbour)
            fprintf(file, "    %u -> %u;\n", i + 1, cur->num);
    }

    fprintf(file, "}\n");
    fclose(file);

    system("dot -Tpng out/graph.gv -o out/graph.png");
    system("xdg-open out/graph.png");

    return OK;
}

int paint_graphviz(graph_t *graph, const uint node,  bool *const visited)
{
    if (!graph->nodes)
        return ERR_EMPTY_GRAPH;

    FILE *file = fopen("out/paint_graph.gv", "w");

    if (!file )
        return ERR_OPEN_FILE;

    fprintf(file, "digraph {\n");

    for (uint i = 0; i < graph->nodes; i++)
    {
        if (!visited[i])
            fprintf(file, "    %u[color=\"red\"];\n", i + 1);
        
        if (i == node - 1)
            fprintf(file, "    %u[color=\"green\"];\n", i + 1);

    }

    for (uint i = 0; i < graph->nodes; i++)
    {
        if (!graph->graph[i])
            fprintf(file, "    %u;\n", i + 1);

        for (node_t *cur = graph->graph[i]; cur; cur = cur->next_neighbour)
            fprintf(file, "    %u -> %u;\n", i + 1, cur->num);
    }

    fprintf(file, "}\n");
    fclose(file);

    system("dot -Tpng out/paint_graph.gv -o out/paint_graph.png");
    system("xdg-open out/paint_graph.png");

    return OK;
}


void print_unreach(bool *arr, const uint size)
{
    uint num = 0;
    for (uint i = 0; i < size; i++)
    {
        if (!arr[i])
            num++;
    }

    if (!num)
        puts("Недостижимых вершин нет!");
    else
        puts("Недостижимые вершины:");

    for (uint i = 0; i < size; i++)
    {
        if (!arr[i])
            printf("%d%s", i + 1, (i == size - 1) ? " " : "\n");
    }
}

int find_nodes(graph_t *graph, const uint node)
{
    int exit_code = OK;
    bool *visited = calloc(graph->nodes, sizeof(bool));

    if (!visited)
        return ERR_MEMORY;

    queue_t queue;
    exit_code = queue_init(&queue, graph->nodes);

    if (exit_code)
        return exit_code;

    exit_code = aq_push(&queue, node);

    if (exit_code)
        return exit_code;
   
    visited[node - 1] = true;

	while(queue.length > 0)
    {
		uint cur_node;
        aq_pop(&queue, &cur_node);

        for (node_t *cur = graph->graph[cur_node - 1]; cur; cur = cur->next_neighbour)
        {
			if (!visited[cur->num - 1])
            {
                aq_push(&queue, cur->num);
				visited[cur->num - 1] = true;
			}
		}
	}

    print_unreach(visited, graph->nodes);

    //paint_graphviz(graph, node, visited);

    free(visited);
    free(queue.array);
    return exit_code;
}

int find_by_node(graph_t *graph)
{
    if (!graph->nodes)
        return ERR_EMPTY_GRAPH;

    puts("Введите номер вершины:");

    uint node;
    int exit_code = read_uint(&node);
    clear_stdin();

    if (exit_code)
        return exit_code;

    if (node <= 0 || node > graph->nodes)
        return ERR_NODE;

    exit_code = find_nodes(graph, node);

    return OK;
}

