#ifndef __TABLE_H__
#define __TABLE_H__
#include<stdint.h>
#include<stdlib.h>
#include<stddef.h>
#include<stdbool.h>
#include "data.h"
#define TABLE_MAX_SIZE 100
extern const uint32_t ROW_SIZE;
#define PAGE_SIZE (4096)
#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)
#define TABLE_MAX_ROWS (ROWS_PER_PAGE * TABLE_MAX_SIZE)
typedef struct{
  int file_descriptor;
  int file_length;
  void* pages[TABLE_MAX_SIZE];
}Pager;
typedef struct{
  uint32_t num_rows;
  Pager* pager;
}Table;
typedef struct{
  uint32_t row_nums;
  Table *table;
  bool end_of_table;
}Cursor;
Cursor *table_start(Table* table);
Cursor *table_end(Table* table);
void *advance_cursor(Cursor* cursor);
void *cursor_value(Cursor* cursor);
static Pager* open_pager(const char* filename);
static void* get_page(Pager* pager, uint32_t page_num);
static void pager_flush(Pager* pager, uint32_t page_num, uint32_t size);
Table *db_open(const char* filename);
void db_close(Table *table);
#endif
