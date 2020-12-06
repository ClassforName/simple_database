#ifndef __INPUTBUFFER_H__
#define __INPUTBUFFER_H__
#include<stdio.h>
typedef struct{
  char *buffer;
  size_t buffer_length;
  ssize_t input_length;
}InputBuffer;
InputBuffer* NewInputerBuffer();
void CloseInputBuffer(InputBuffer *inputBuffer);
int ReadInput(InputBuffer* inputBuffer);
#endif
