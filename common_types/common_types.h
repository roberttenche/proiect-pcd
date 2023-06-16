#pragma once

#include <stdlib.h>
#include <sys/types.h>

typedef struct header_s
{
  char file_extension[20];
  size_t file_size;
  unsigned char blur_multiplier;
  unsigned char upscale_multiplier;
  unsigned char downscale_multiplier;
} header_t;

typedef struct tcp_connection_s
{
  pid_t child_pid;
  int child_pipe;
  pthread_t timing_thread;
  int lifetime;
  char ip_addr[16];
} tcp_connection_t;
