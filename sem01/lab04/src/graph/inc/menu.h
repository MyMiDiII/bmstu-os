#ifndef __MENU_H__

#define __MENU_H__

#define MAX_MENU_ITEM_LEN 2

#include "graph.h"

void print_menu(void);

int choose_action(short int *const action);

void print_error_message(const int code);

int do_action(const short int action, graph_t *graph);

#endif
