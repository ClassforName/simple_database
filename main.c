#include<stdbool.h>
#include "InputBuffer.h"
#include "LinuxAPIForWindows.h"
#include "MetaCommandCompile.h"
#include "StatementCompile.h"

void PrintPrompt()
{
  printf("db >");
}

int main(int argc, char *argv[])
{
  InputBuffer *inputBuffer = NewInputerBuffer();
  Table *table = new_table();
  while(true) {
    PrintPrompt();
    ReadInput(inputBuffer);
    if(inputBuffer->buffer[0] == '.'){
      switch (do_meta_command(inputBuffer, table)) {
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECONIZED_COMMAND):
          printf("Unreconsized command\n");
          continue;
      }
    }

    Statement statement;
    switch (prepare_statment(inputBuffer, &statement)) {
      case (PREPARE_SUCESS):
        break;
      case (PREPARE_SYNTAX_ERROR):
        printf("Synax error can not parser the statement\n");
        break;
      case(PREPARE_UNRECONIZED_STATMENT):
        printf("unrecognized keyword in start of '%s'\n", inputBuffer->buffer);
        break;
    }
    switch(excute_statement(&statement)){
       case EXCUTE_TABLE_FULL:
        printf("table is full\n");
        break;
       case EXCUTE_SUCESS:
        printf("excute statement success\n");
        break;
    }
  }
  return 0;
}
