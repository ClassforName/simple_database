#include"Node.h"
#include"InputBuffer.h"
#include "StatementCompile.h"

static struct{
  PrepareResult prePareResult;
  char *log;
}prePareExcuteCodeLogMap[] = {
  {PREPARE_SYNTAX_ERROR, "systax error"},
  {PREPARE_UNRECONIZED_STATMENT, "unreconized statment"},
  {PREPARE_STRING_TOO_LONG, "string too long"},
  {PREPAER_INVALID_ID, "invalid id"},
};

static struct{
  ExcuteResult excuteResult;
  char *log;
}excuteCodeLogMap[] = {
  {EXCUTE_TABLE_FULL, "excute error table is null"},
  {EXCUTE_DUPLICATE_KEY, "excute error duplicate key"}
};

static struct{
  StatementType operationType;
  char *log;
}excuteOperationLogMap[] = {
  {STATMENT_INSERT, "insert"},
  {STATMENT_SELECT, "select"}
};

static struct {
  StatementType excuteType;
  void (*prepareExcuteHandle)(InputBuffer* inputerBuffer, Statement* statement);
  void (*excuteHandle)(Statement* statement, Table *table);
  void (*afterExcuteHandle)(Statement* statement, Table *table);
}processTab[] = {
  {STATMENT_INSERT, prepare_insert, excute_insert, after_insert},
  {STATMENT_SELECT, prepare_select, excute_select, after_select},
  {STATMENT_OPERATION_INVALID, NULL, NULL, NULL}
};

static void prepare_insert(InputBuffer* inputerBuffer, Statement* statement)
{
  char* keyWord = strtok(inputerBuffer->buffer, " ");
  char* id_string = strtok(NULL, " ");
  char* userName = strtok(NULL, " ");
  char* email = strtok(NULL, " ");
  if(keyWord == NULL || id_string == NULL || userName == NULL || email == NULL){
    statement->prepareResult  = PREPARE_SYNTAX_ERROR;
  }

  int id = atoi(id_string);
  if(id < 0){
    statement->prepareResult = PREPAER_INVALID_ID;
  }
  
  if(strlen(userName) > COLUMN_USERNAME_SIZE || strlen(email) > COLUMN_EMAIL_SIZE) {
    statement->prepareResult =  PREPARE_STRING_TOO_LONG;
  }

  statement->row_to_insert.id = id;
  strcpy(statement->row_to_insert.username, userName);
  strcpy(statement->row_to_insert.email, email);

  statement->prepareResult =  PREPARE_SUCESS;
}

static void prepare_select(InputBuffer* inputerBuffer, Statement* statement)
{
  statement->prepareResult = PREPARE_SUCESS;
}

static void excute_insert(Statement *statement, Table *table)
{
  if(statement->prepareResult != PREPARE_SUCESS){
    return;
  }
  void *node = get_page(table->pager, table->root_page_num);
  uint32_t node_num = *(leaf_node_num_cells(node));
  if(node_num >= get_leaf_node_max_cells()){
    statement->excuteResult = EXCUTE_TABLE_FULL;
    return;
  }
  Row *row_to_insert =  &(statement->row_to_insert);
  uint32_t key_to_insert = row_to_insert->id;
  Cursor* cursor = table_find(row_to_insert->id, table);
  if(cursor->cell_num < node_num){
    uint32_t ket_at_index = *leaf_node_key(node, cursor->cell_num);
    if(ket_at_index == key_to_insert){
      statement->excuteResult = EXCUTE_DUPLICATE_KEY;
      return;
    }
  }
  leaf_node_insert(cursor, statement->row_to_insert.id, row_to_insert);
  free(cursor);
  statement->excuteResult = EXCUTE_SUCESS;
}

static void excute_select(Statement *statement, Table *table)
{
  Cursor* cursor = table_start(table);
  Row row;
  while(!(cursor->end_of_table)){
    deserialize_row(cursor_value(cursor), &row);
    print_row(&row);
    advance_cursor(cursor);
  }
  free(cursor);
  statement->excuteResult = EXCUTE_SUCESS;
}

static void after_insert(Statement *statement, Table *table)
{
  excuteLog(statement);
}

static void after_select(Statement *statement, Table *table)
{
  excuteLog(statement);
}

static void after_invalidOperation(Statement *statement, Table *table)
{
  excuteLog(statement);
}

static void excuteLog(Statement *statement)
{
  if(statement->type == STATMENT_OPERATION_INVALID){
    printf("operation abnormal\n");
    return;
  }

  char *operation_s = NULL;
  for(uint32_t i = 0; i < sizeof(excuteOperationLogMap) / sizeof(excuteOperationLogMap[0]); i++){
      if(statement->type == excuteOperationLogMap[i].operationType){
        operation_s = excuteOperationLogMap[i].log;
        break;
      }
  }

  for(uint32_t i = 0; i < sizeof(prePareExcuteCodeLogMap) / sizeof(prePareExcuteCodeLogMap[0]); i++){
    if(statement->prepareResult == prePareExcuteCodeLogMap[i].prePareResult){
      printf("[%s] prepare parse [%s]\n", operation_s, prePareExcuteCodeLogMap[i].log);
      return;
    }
  }

  for(uint32_t i = 0; i < sizeof(excuteCodeLogMap) / sizeof(excuteCodeLogMap[0]); i++){
    if(statement->excuteResult == excuteCodeLogMap[i].excuteResult){
      printf("[%s] excute [%s]\n", operation_s, excuteCodeLogMap[i].log);
      return;
    }
  }
}

static void ParseOperationType(InputBuffer* inputBuffer, Statement* statement)
{
  if(strncmp(inputBuffer->buffer, "insert", 6) == 0){
    statement->type = STATMENT_INSERT;
  }
  if(strncmp(inputBuffer->buffer, "select", 6) == 0){
    statement->type = STATMENT_SELECT;
  }
  statement->type = STATMENT_OPERATION_INVALID;
}

void excuteOperation(InputBuffer *inputBuffer, Table* table){
  Statement statement = {0};
  ParseOperationType(inputBuffer, &statement);
  for(uint32_t i = 0; i < sizeof(processTab) / sizeof(processTab[0]); i++){
      if(statement.type == processTab[i].excuteType){
        if(processTab[i].prepareExcuteHandle != NULL){
          processTab[i].prepareExcuteHandle(inputBuffer, &statement);
        }
        if(processTab[i].excuteHandle != NULL){
          processTab[i].excuteHandle(&statement, table);
        }
        if(processTab[i].afterExcuteHandle != NULL){
          processTab[i].afterExcuteHandle(&statement, table);
        }
        return;
      }
  }
}
