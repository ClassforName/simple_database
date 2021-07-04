#ifndef __STATEMENTCOMPILE_H__
#define __STATEMENTCOMPILE_H__
#include "InputBuffer.h"
#include "data.h"
#include "table.h"
#include<string.h>
typedef enum{
  PREPARE_SUCESS,
  PREPARE_SYNTAX_ERROR,
  PREPARE_UNRECONIZED_STATMENT,
  PREPARE_STRING_TOO_LONG,
  PREPAER_INVALID_ID
}PrepareResult;

typedef enum{
  EXCUTE_SUCESS,
  EXCUTE_TABLE_FULL,
  EXCUTE_DUPLICATE_KEY
}ExcuteResult;

typedef enum{
  STATMENT_INSERT,
  STATMENT_SELECT,
  STATMENT_OPERATION_INVALID
}StatementType;

typedef struct{
  StatementType type;
  Row row_to_insert;
  PrepareResult prepareResult;
  ExcuteResult excuteResult;
}Statement;

void excuteOperation(InputBuffer *inputBuffer, Table* table);
static void excute_insert(Statement *statement, Table *table);
static void excute_select(Statement *statement, Table *table);
static void prepare_insert(InputBuffer* inputerBuffer, Statement* statement);
static void prepare_select(InputBuffer* inputerBuffer, Statement* statement);
static void after_insert(Statement *statement, Table *table);
static void after_select(Statement *statement, Table *table);
static void after_invalidOperation(Statement *statement, Table *table);
static void excuteLog(Statement *statement);
static void ParseOperationType(InputBuffer* inputBuffer, Statement* statement);
#endif
