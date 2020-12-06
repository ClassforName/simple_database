#include "table.h"
void *row_slot(Table *table, uint32_t row_num)
{
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  void *page = table->pages[page_num];
  if (page == NULL) {
    page = table->pages[page_num] = malloc(PAGE_SIZE);
  }

  uint32_t row_offset = row_num % PAGE_SIZE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}

Table *new_table(){
  Table *table = (Table *)malloc(sizeof(Table));
  table->num_rows = 0;
  for(int i = 0; i<TABLE_MAX_SIZE; i++){
    table->pages[i] = NULL;
  }
  return table;
}

void free_table(Table *table){
  for(int i = 0; i < table->num_rows; i++){
    free(table->pages[i]);
  }
  free(table);
}
