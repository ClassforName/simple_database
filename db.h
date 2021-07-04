#ifndef __DB_H__
#define __DB_H__
#include "table.h"
#include "InputBuffer.h"
#define BACK_FILE "dbfile"
static Table *db_open(const char* filename);
void db_close(Table *table);
static Table* DataBaseInit();
void DataBaseRun();
#endif