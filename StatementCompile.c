#include"Node.h"
#include"InputBuffer.h"
#include "StatementCompile.h"
PrepareResult prepare_insert(InputBuffer* inputerBuffer, Statement* statement){
  statement->type = STATMENT_INSERT;
  char* keyWord = strtok(inputerBuffer->buffer, " ");
  char* id_string = strtok(NULL, " ");
  char* userName = strtok(NULL, " ");
  char* email = strtok(NULL, " ");
  if(keyWord == NULL || id_string == NULL || userName == NULL || email == NULL){
    return PREPARE_SYNTAX_ERROR;
  }

  int id = atoi(id_string);
  if(id < 0){
    return PREPAER_INVALID_ID;
  }
  
  if(strlen(userName) > COLUMN_USERNAME_SIZE || strlen(email) > COLUMN_EMAIL_SIZE) {
    return PREPARE_STRING_TOO_LONG;
  }

  statement->row_to_insert.id = id;
  strcpy(statement->row_to_insert.username, userName);
  strcpy(statement->row_to_insert.email, email);

  return PREPARE_SUCESS;
}

PrepareResult prepare_statment(InputBuffer* inputBuffer, Statement* statement)
{
  if(strncmp(inputBuffer->buffer, "insert", 6) == 0){
      return prepare_insert(inputBuffer, statement);
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
  void *node = get_page(table->pager, table->root_page_num);
  uint32_t node_num = *(leaf_node_num_cells(node));
  Row *row_to_insert =  &(statment->row_to_insert);
  uint32_t key_to_insert = row_to_insert->id;
  Cursor* cursor = table_find(row_to_insert->id, table);
  if(cursor->cell_num < node_num){
    uint32_t ket_at_index = *leaf_node_key(node, cursor->cell_num);
    if(ket_at_index == key_to_insert){
      return EXCUTE_DUPLICATE_KEY;
    }
  }
  leaf_node_insert(cursor, statment->row_to_insert.id, row_to_insert);
  free(cursor);
  return EXCUTE_SUCESS;
}

ExcuteResult excute_select(Statement *statement, Table *table)
{
  Cursor* cursor = table_start(table);
  Row row;
  while(!(cursor->end_of_table)){
    deserialize_row(cursor_value(cursor), &row);
    print_row(&row);
    advance_cursor(cursor);
  }
  free(cursor);
  return EXCUTE_SUCESS;
}
