#ifndef __METACOMMANDCOMPILE_H__
#define __METACOMMANDCOMPILE_H__
#include<string.h>
#include<stddef.h>
#include "InputBuffer.h"
#include "Table.h"
typedef enum{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECONIZED_COMMAND
}MetaCommandResult;
MetaCommandResult do_meta_command(InputBuffer* inputBuffer, Table *table);
#endif
