#include "common_types.h"

#include <stdio.h>
#include "time.h"

extern tcp_connection_t* connections;

int timing_arr[MAX_TCP_CONNECTIONS];

void* my_timer(void* args)
{
  int current_connection_idx = *((int*)args);

  unsigned int time_elapsed_ms = 0;

  struct timespec start_time, end_time;

  timing_arr[current_connection_idx] = 1;

  clock_gettime(CLOCK_MONOTONIC, &start_time);  // Get the starting time

  while (timing_arr[current_connection_idx] == 1)
  {
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    time_elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000LL + 
                    (end_time.tv_nsec - start_time.tv_nsec) / 1000000LL;

    connections[current_connection_idx].lifetime = time_elapsed_ms;
  }

  return (void*)args;
}