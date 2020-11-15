#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct{
  char *buffer;
  size_t buffer_length;
  ssize_t input_length;
}InputBuffer;

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}

void PrintPrompt()
{
  printf("db >");
}

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

int main(int argc, char *argv[])
{
  InputBuffer *inputBuffer = NewInputerBuffer();
  while(true) {
    PrintPrompt();
    ReadInput(inputBuffer);
    if (strcmp(inputBuffer->buffer, ".exit") == 0) {
        CloseInputBuffer(inputBuffer);
        exit(EXIT_SUCCESS);
    }

    printf("unrecognized command %s\n", inputBuffer->buffer);
  }
  return 0;
}
