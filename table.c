#include <fcntl.h>
#include <sys/stat.h>
#include "Node.h"
#include "table.h"

void* get_page(Pager* pager, uint32_t page_num){
  if(page_num > TABLE_MAX_SIZE){
    printf("try to fetch page number out of bounds. %d > %d", page_num, TABLE_MAX_SIZE);
    exit(EXIT_FAILURE);
  }
  if(pager->pages[page_num] == NULL){
    void* page = malloc(PAGE_SIZE);
    uint32_t num_pages = pager->file_length / PAGE_SIZE;
    if(page_num <= num_pages){
      lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
      ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
      if(bytes_read == -1){
        printf("error reading file: %d \n", errno);
        exit(EXIT_FAILURE);
      }
      pager->pages[page_num] = page;

      if(page_num > pager->nums_page){
        pager->nums_page += page_num + 1;
      }

    }
  }
  return pager->pages[page_num];
}

static void pager_flush(Pager* pager, uint32_t page_num){
  if(pager->pages[page_num] == NULL){
    printf("try to flush null page \n");
    exit(EXIT_FAILURE);
  }

  off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
  if(offset == -1){
    printf("error seeking: %d \n", errno);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes_write = write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);
  if(bytes_write == -1){
    printf("error wiriting: %d \n", errno);
    exit(EXIT_FAILURE);
  }
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

void *cursor_value(Cursor* cursor)
{
  uint32_t page_num = cursor->page_num;
  void* node = get_page(cursor->table->pager, page_num);
  return leaf_node_value(node, cursor->cell_num);
}

static Pager* open_pager(const char* filename){
  int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
  if(fd == -1){
    printf("unable to open file \n");
    exit(EXIT_FAILURE);
  }
  off_t file_length = lseek(fd, 0, SEEK_END);
  Pager* pager = malloc(sizeof(Pager));
  pager->file_descriptor = fd;
  pager->file_length = file_length;
  pager->nums_page = file_length / PAGE_SIZE;
  if(file_length % PAGE_SIZE){
    printf("DB file is not whole nums of pages. Corrupt file \n");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < TABLE_MAX_SIZE; i++){
    pager->pages[i] = NULL;
  }
  return pager;
}

Table *db_open(const char* filename){
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

Cursor *table_start(Table* table){
  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = table->root_page_num;
  cursor->cell_num = 0;
  void *root_node = get_page(table->pager, table->root_page_num);
  uint32_t num_cells = *leaf_node_num_cells(root_node);
  cursor->end_of_table = (num_cells == 0);
  return cursor;
}

Cursor *table_end(Table* table){
  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = table->root_page_num;
  void *root_node = get_page(table->pager, table->root_page_num);
  uint32_t cell_nums = *leaf_node_num_cells(root_node);
  cursor->cell_num = cell_nums;
  cursor->end_of_table = true;
  return cursor;
}

void *advance_cursor(Cursor* cursor){
    uint32_t page_num = cursor->page_num;
    void *node = get_page(cursor->table->pager, page_num);
    cursor->cell_num += 1;
    if(cursor->cell_num >= (*leaf_node_num_cells(node))){
      cursor->end_of_table = true;
    }
}