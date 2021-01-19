#include "MetaCommandCompile.h"
MetaCommandResult do_meta_command(InputBuffer* inputBuffer, Table *table)
{
  if(strcmp(inputBuffer->buffer, ".exit") == 0){
    db_close(table);
    exit(EXIT_SUCCESS);
  }else{
    return META_COMMAND_UNRECONIZED_COMMAND;
  }
}
