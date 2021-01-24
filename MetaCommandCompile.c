#include "MetaCommandCompile.h"
#include "Node.h"
MetaCommandResult do_meta_command(InputBuffer* inputBuffer, Table *table)
{
  if(strcmp(inputBuffer->buffer, ".exit") == 0){
    db_close(table);
    exit(EXIT_SUCCESS);
  }else if(strcmp(inputBuffer->buffer, ".constants") == 0){
    printf("Constants:\n");
    print_constant();
    return META_COMMAND_SUCCESS;
  }else if(strcmp(inputBuffer->buffer, ".dbTree") == 0){
    printf("Tree:\n");
    print_leaf_node(get_page(table->pager, 0));
    return META_COMMAND_SUCCESS;
  }else{
    return META_COMMAND_UNRECONIZED_COMMAND;
  }
}
