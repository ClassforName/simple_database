#ifndef __METACOMMANDCOMPILE_H__
#define __METACOMMANDCOMPILE_H__
#include<string.h>
#include<stddef.h>
#include "InputBuffer.h"
#include "table.h";
static void do_meta_command_exit(InputBuffer* inputBuffer, Table *table);
static void do_meta_command_constants(InputBuffer *inputBuffer, Table* table);
static void do_meta_command_dbTree(InputBuffer* inputBuffer, Table* table);
void do_meta_command(InputBuffer* inputBuffer, Table *table);
#endif
