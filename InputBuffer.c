#include<stdlib.h>
#include<string.h>
#include "InputBuffer.h"
InputBuffer* NewInputerBuffer()
{
  InputBuffer *inputBuffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  inputBuffer->buffer = NULL;
  inputBuffer->input_length = 0;
  inputBuffer->buffer_length = 0;
  return inputBuffer;
}

void CloseInputBuffer(InputBuffer *inputBuffer)
{
  free(inputBuffer->buffer);
  free(inputBuffer);
}

int ReadInput(InputBuffer* inputBuffer)
{
  ssize_t byteCount  = getline(&(inputBuffer->buffer),
                               &(inputBuffer->buffer_length), stdin);
  if(byteCount == -1) {
    printf("read input error \n");
    exit(EXIT_FAILURE);
  }
  inputBuffer->input_length = byteCount - 1;
  inputBuffer->buffer[byteCount - 1] = 0;
  return 0;
}
