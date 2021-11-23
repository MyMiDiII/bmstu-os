#include <stdio.h>
#include <stdlib.h>

#include "menu.h"
#include "err.h"
#include "io.h"
#include "graph.h"

void print_menu(void)
{
    puts("");
    puts("          ГРАФЫ");
    puts("");
    puts("1  - Задать граф");
    puts("2  - Посмотреть заданный граф");
    puts("3  - Найти вершины, недостижимые из заданной");
    puts("0  - Выход");
    puts("");
}

void print_error_message(const int code)
{
    switch (code)
    {
        case (ERR_WRONG_ACTION):
            puts("\nВведенный код не соответствует ни одному действию.");
            puts("Попробуйте ещё раз.\n");
            break;
        case (ERR_READ_ACTION):
            puts("\nНекорректный ввод кода действия.");
            puts("Попробуйте ещё раз.\n");
            break;
        case (ERR_NONINT):
            puts("\nОшибка при чтении числа!\n");
            break;
        case (ERR_NONUINT):
            puts("\nВведенное число выходит за допустимый диапазон!\n");
            break;
        case (ERR_OPEN_FILE):
            puts("\nОшибка при открытии файла!\n");
            break;
        case (ERR_EDGE):
            puts("\nОшибка при чтении ребра!\n");
            break;
        case (ERR_EMPTY_GRAPH):
            puts("\nГраф не задан!\n");
            break;
        case (ERR_NODE):
            puts("\nНомер вершины выходит за допустимый диапазон!\n");
            break;
        default:
            break;
    }
}

int choose_action(short int *const action)
{
    puts("Выберете пункт меню: ");

    char str[MAX_MENU_ITEM_LEN + 2];

    if (read_str(str, MAX_MENU_ITEM_LEN + 3, stdin))
        return ERR_READ_ACTION;

    char *end_prt;
    long int long_str = strtol(str, &end_prt, 10);

    if (*end_prt != '\0')
        return ERR_WRONG_ACTION;
    
    if (long_str < 0 || long_str > 3)
        return ERR_WRONG_ACTION;

    *action = (short int) long_str;

    return OK;
}

int do_action(const short int action, graph_t *graph)
{
    int exit_code = OK;

    switch (action)
    {
        case 1:
            free_graph(graph);
            graph->graph = NULL;
            graph->nodes = 0;
            exit_code = read_graph(graph);

            if (exit_code)
            {
                free_graph(graph);
                graph->graph = NULL;
                graph->nodes = 0;
            }

            break;

        case 2:
            exit_code = graphviz(graph);
            break;

        case 3:
            exit_code = find_by_node(graph);
            break;

        default:
            free_graph(graph);

            puts("\nСпасибо за использование программы!");
            puts("Автор:  МАСЛОВА МАРИНА");
            puts("Группа: ИУ7-33Б");
            exit(EXIT_SUCCESS);
            break;
    }

    return exit_code;
}
