#include "MetaCommandCompile.h"
#include "Node.h"
#include "db.h"
static struct {
  char* commandType;
  void (*commandHandle)(InputBuffer* inputBuffer, Table *table);
}commandParseTab[] = {
  {".exit", do_meta_command_exit},
  {".constants", do_meta_command_constants},
  {".dbTree", do_meta_command_dbTree}
};

static void do_meta_command_exit(InputBuffer* inputBuffer, Table *table)
{
  db_close(table);
  exit(EXIT_SUCCESS);
}

static void do_meta_command_constants(InputBuffer *inputBuffer, Table* table)
{
  print_constant();
}

static void do_meta_command_dbTree(InputBuffer* inputBuffer, Table* table)
{
  print_leaf_node(get_page(table->pager, 0));
}

void do_meta_command(InputBuffer* inputBuffer, Table *table)
{
  for(u_int32_t i = 0; i < sizeof(commandParseTab) / sizeof(commandParseTab[0]); i++){
    if(strcmp(inputBuffer->buffer, commandParseTab[i].commandType) == 0){
      commandParseTab[i].commandHandle(inputBuffer, table);
      return;
    }
  }
  printf("Unreconsized command\n");
}
