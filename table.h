#ifndef __TABLE_H__
#define __TABLE_H__
#include<stdint.h>
#include<stdlib.h>
#include<stddef.h>
#include<stdbool.h>
#include "data.h"
#define TABLE_MAX_SIZE 100
#define PAGE_SIZE (4096)
typedef struct{
  int file_descriptor;
  int file_length;
  int nums_page;
  void* pages[TABLE_MAX_SIZE];
}Pager;

typedef struct{
  uint32_t num_rows;
  Pager* pager;
  uint32_t root_page_num;
}Table;

typedef struct{
  uint32_t cell_num;
  uint32_t page_num;
  Table *table;
  bool end_of_table;
}Cursor;

Cursor *table_start(Table* table);
Cursor *table_end(Table* table);
void *advance_cursor(Cursor* cursor);
void *cursor_value(Cursor* cursor);
Pager* open_pager(const char* filename);
void* get_page(Pager* pager, uint32_t page_num);
void pager_flush(Pager* pager, uint32_t page_num);
Cursor *table_find(uint32_t key, Table* table);
static Cursor* leaf_node_find(Table *table, uint32_t page_num, uint32_t key);
#endif
