#ifndef __TABLE_H__
#define __TABLE_H__
#include<stdint.h>
#include<stdlib.h>
#include<stddef.h>
#include "data.h"
#define TABLE_MAX_SIZE 100
extern const uint32_t ROW_SIZE;
#define PAGE_SIZE (4096)
#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)
#define TABLE_MAX_ROWS (ROWS_PER_PAGE * TABLE_MAX_SIZE)
typedef struct{
  uint32_t num_rows;
  void* pages[TABLE_MAX_SIZE];
}Table;
void *row_slot(Table *table, uint32_t row_num);
Table *new_table();
void free_table(Table *table);
#endif
