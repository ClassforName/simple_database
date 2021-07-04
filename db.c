#include "db.h"

static Table *db_open(const char* filename){
  Pager* pager = open_pager(filename);
  Table *table = (Table *)malloc(sizeof(Table));
  table->root_page_num = 0;
  table->pager = pager;
  if(pager->nums_page == 0){
    void *root_node = get_page(pager, 0);
    initialize_leaf_node(root_node);
  }
  return table;
}

void db_close(Table* table){
  Pager* pager = table->pager;
  for(uint32_t i = 0; i <= pager->nums_page; i++){
    if(pager->pages[i] == NULL){
      continue;
    }
    pager_flush(pager, i);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
  }

  int result = close(pager->file_descriptor);
  if(result == -1){
    printf("Error close file. \n");
    exit(EXIT_FAILURE);
  }
  for(uint32_t i = 0; i < TABLE_MAX_SIZE; i++){
    void* page = pager->pages[i];
    if(page){
      free(page);
      pager->pages[i] = NULL;
    }
  }
  free(pager);
  free(table);
}

static Table* DataBaseInit()
{
  Table* table = db_open(BACK_FILE);
  return table;
}

void DataBaseRun()
{
  Table *table = DataBaseInit();
  InputBuffer *inputBuffer = NewInputerBuffer();
  while(true) {
    PrintPrompt();
    ReadInput(inputBuffer);
    if(inputBuffer->buffer[0] == '.'){
      do_meta_command(inputBuffer, table);
      continue;
    }
    excuteOperation(inputBuffer, table);
  }
}
