#ifndef NODE_H
#define NODE_H
#include<stddef.h>
#include<stdint.h>
#include<string.h>
#include"table.h"
typedef enum {NODE_INTERNAL, NODE_LEAF} NodeType;

uint32_t *leaf_node_num_cells(void *node);
uint32_t *leaf_node_cell(void *node, uint32_t cell_nums);
uint32_t *leaf_node_key(void* node, uint32_t cell_nums);
uint32_t *leaf_node_value(void* node, uint32_t cell_nums);
uint32_t get_leaf_node_max_cells();
void initialize_leaf_node(void *node);
void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value);
void print_constant();
void print_leaf_node();
#endif