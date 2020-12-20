#ifndef __STATEMENTCOMPILE_H__
#define __STATEMENTCOMPILE_H__
#include "InputBuffer.h"
#include "data.h"
#include "table.h"
#include<string.h>
typedef enum{
  PREPARE_SUCESS,
  PREPARE_SYNTAX_ERROR,
  PREPARE_UNRECONIZED_STATMENT
}PrepareResult;
typedef enum{
  EXCUTE_SUCESS,
  EXCUTE_TABLE_FULL
}ExcuteResult;
typedef enum{
  STATMENT_INSERT,
  STATMENT_SELECT,
  STATMENT_OPERATION_INVALID
}StatementType;
typedef struct{
  StatementType type;
  Row row_to_insert;
}Statement;
ExcuteResult excute_statement(Statement* statement, Table *table);
ExcuteResult excute_insert(Statement *statement, Table *table);
ExcuteResult excute_select(Statement *statement, Table *table);
PrepareResult prepare_statment(InputBuffer* inputBuffer, Statement* statement);
#endif
