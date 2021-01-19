#ifndef __DATA_H__
#define __DATA_H__
#include<stdint.h>
#include<stdio.h>
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct{
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE + 1];
  char email[COLUMN_EMAIL_SIZE + 1];
}Row;
#define size_of_attribute(struct, attribute) sizeof(((struct*)0)->attribute)
void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
void print_row(Row *row);
#endif
