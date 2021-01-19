#include <fcntl.h>
#include <sys/stat.h>
#include "table.h"

static void* get_page(Pager* pager, uint32_t page_num){
  if(page_num > TABLE_MAX_SIZE){
    printf("try to fetch page number out of bounds. %d > %d", page_num, TABLE_MAX_SIZE);
    exit(EXIT_FAILURE);
  }
  if(pager->pages[page_num] == NULL){
    void* page = malloc(PAGE_SIZE);
    uint32_t num_pages = pager->file_length / PAGE_SIZE;
    if(pager->file_length % PAGE_SIZE){
      num_pages += 1;
    }
    if(page_num <= num_pages){
      lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
      ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
      if(bytes_read == -1){
        printf("error reading file: %d \n", errno);
        exit(EXIT_FAILURE);
      }
      pager->pages[page_num] = page;
    }
  }
  return pager->pages[page_num];
}

static void pager_flush(Pager* pager, uint32_t page_num, uint32_t size){
  if(pager->pages[page_num] == NULL){
    printf("try to flush null page \n");
    exit(EXIT_FAILURE);
  }

  off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
  if(offset == -1){
    printf("error seeking: %d \n", errno);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes_write = write(pager->file_descriptor, pager->pages[page_num], size);
  if(bytes_write == -1){
    printf("error wiriting: %d \n", errno);
    exit(EXIT_FAILURE);
  }
}

void db_close(Table* table){
  Pager* pager = table->pager;
  uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;
  for(uint32_t i = 0; i < num_full_pages; i++){
    if(pager->pages[i] == NULL){
      continue;
    }
    pager_flush(pager, i, PAGE_SIZE);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
  }

  uint32_t num_addtional_rows = table->num_rows % ROWS_PER_PAGE;
  if(num_addtional_rows > 0){
    uint32_t page_num = num_full_pages;
    if(pager->pages[page_num] != NULL){
      pager_flush(pager, page_num, num_addtional_rows * ROW_SIZE);
      free(pager->pages[page_num]);
      pager->pages[page_num] == NULL;
    }
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

void *row_slot(Table *table, uint32_t row_num)
{
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  void* page = get_page(table->pager, page_num);
  uint32_t row_offset = row_num % PAGE_SIZE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
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
  for(int i = 0; i < TABLE_MAX_SIZE; i++){
    pager->pages[i] = NULL;
  }
  return pager;
}

Table *db_open(const char* filename){
  Pager* pager = open_pager(filename);
  int row_nums = pager->file_length / ROW_SIZE;
  Table *table = (Table *)malloc(sizeof(Table));
  table->num_rows = row_nums;
  table->pager = pager;
  return table;
}
