#include "data.h"
#include<string.h>
#define ID_SIZE (size_of_attribute(Row, id))
#define  USERNAME_SIZE (size_of_attribute(Row, username))
#define  EMAIL_SIZE (size_of_attribute(Row, email))
#define  ID_OFFSET 0
#define  USERNAME_OFFSET (ID_OFFSET + ID_SIZE)
#define  EMAIL_OFFSET (USERNAME_OFFSET + USERNAME_SIZE)
#define  ROW_SIZE (ID_SIZE + EMAIL_SIZE + USERNAME_SIZE)
void serialize_row(Row *source, void* destination)
{
  memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
  memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
  memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row *destination)
{
  memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
  memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
  memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void print_row(Row *row){
  printf("id=%d username=%s email=%s\n", row->id, row->username, row->email);
}

const uint32_t get_row_size(){
  return ROW_SIZE;
}
