#include "my_io.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[])
{
  size_t buff_size = 1;
  char* buff = (char*)malloc(buff_size);

  buff_size = myRead(argv[1], &buff, buff_size);

  return 0;
}
