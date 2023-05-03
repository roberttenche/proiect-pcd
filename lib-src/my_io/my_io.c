#include "my_io.h"

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>


size_t myRead(const char* file_path, char** buffer, size_t buffer_size)
{
  size_t total_size = 0u;
  int file;

  *buffer = (char*)malloc(buffer_size + 1u);

  // open file with only reading permission
  file = open(file_path, O_RDONLY);

  if (file < 0)
  {
    printf("File not accessbile: %s\n", file_path);
    return 0u;
  }

  size_t read_count = 0u;

  while (1u)
  {
    if (total_size >= buffer_size)
    {
      *buffer = (char*)realloc(*buffer, total_size + buffer_size + 1u);
    }

    //                      read starting at index total_size
    read_count = read(file, &((*buffer)[total_size]), buffer_size);

    if (read_count == -1)
    {
      free(*buffer);
      break;
    }

    total_size += read_count;

    if (read_count == 0u)
    {
      (*buffer)[total_size] = '\0';
      break;
    }
  }

  file = close(file);

  if (file < 0)
  {
    printf("Error closing file: %s\n", file_path);
    return 0u;
  }

  return total_size;
}

size_t myWrite(const char* file_path, char* buffer, size_t buffer_size)
{
  // function assumes param [buffer] has memory allocated to it

  int file;

  // open file with only writing permission, create if not exist
  file = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

  if (file < 0)
  {
    printf("File not accessbile: %s\n", file_path);
    return 0u;
  }

  if (buffer_size <= 0)
  {
    printf("Invalid buffer size provided: %lu\n", buffer_size);
    return 0u;
  }

  size_t written_bytes = 0uL;
  size_t total_size = 0uL;

  while (buffer_size > 0u)
  {

    written_bytes = write(file, (void*)buffer, buffer_size);

    buffer_size -= written_bytes;

    total_size += written_bytes;
  }

  file = close(file);

  if (file < 0)
  {
    printf("Error closing file: %s\n", file_path);
    return 0u;
  }

  return total_size;
}