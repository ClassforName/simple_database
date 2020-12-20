#include"InputBuffer.h"
#include "StatementCompile.h"
PrepareResult prepare_statment(InputBuffer* inputBuffer, Statement* statement)
{
  if(strncmp(inputBuffer->buffer, "insert", 6) == 0){
    statement->type = STATMENT_INSERT;
    int args_assigned = sscanf(inputBuffer->buffer,"insert %d %s %s",
                               &(statement->row_to_insert.id),
                               &(statement->row_to_insert.username),
                               &(statement->row_to_insert.email));
    if(args_assigned < 3){
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCESS;
  }
  if(strcmp(inputBuffer->buffer, "select") == 0){
    statement->type = STATMENT_SELECT;
    return PREPARE_SUCESS;
  }
  statement->type = STATMENT_OPERATION_INVALID;
  return PREPARE_UNRECONIZED_STATMENT;
}

ExcuteResult excute_statement(Statement* statement, Table* table){
    switch (statement->type){
      case (STATMENT_INSERT):
        //printf("This is where we would do an insert\n");
        return excute_insert(statement, table);
        break;
      case (STATMENT_SELECT):
        //printf("This is where we would do an select\n");
        return excute_select(statement, table);
        break;
    }
}

ExcuteResult excute_insert(Statement *statment, Table *table)
{
  if(table->num_rows >= TABLE_MAX_ROWS) {
      return EXCUTE_TABLE_FULL;
  }
  void *insert_handle = row_slot(table, ((table->num_rows)++));
  serialize_row(&(statment->row_to_insert), insert_handle);
  return EXCUTE_SUCESS;
}

ExcuteResult excute_select(Statement *statement, Table *table)
{
  Row row;
  for(int i = 0; i < table->num_rows; i++){
    deserialize_row(row_slot(table, i), &row);
    print_row(&row);
  }
  return EXCUTE_SUCESS;
}
