#include "sig_handler.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include "my_timer.hpp"
#include "udp_handler.hpp"

#include "common_types.h"

bool interrupted_by_sig_handler = false;

extern tcp_connection_t* connections;

static void sigusr1_handler(int sig_num, siginfo_t* si, void* ignored);
static void sigusr2_handler(int sig_num, siginfo_t* si, void* ignored);
static void sigio_handler(int sig_num, siginfo_t* si, void* ignored);

int init_sig_handlers()
{
  int ret_value = -1;

  // SIGUSR1
  struct sigaction sa_SIGUSR1;
  memset(&sa_SIGUSR1, 0, sizeof(sa_SIGUSR1));
  sa_SIGUSR1.sa_sigaction = sigusr1_handler;
  sa_SIGUSR1.sa_flags = SA_SIGINFO;
  ret_value = sigaction(SIGUSR1, &sa_SIGUSR1, NULL);

  // SIGUSR2
  struct sigaction sa_SIGUSR2;
  memset(&sa_SIGUSR2, 0, sizeof(sa_SIGUSR2));
  sa_SIGUSR2.sa_sigaction = sigusr2_handler;
  sa_SIGUSR2.sa_flags = SA_SIGINFO;
  ret_value = sigaction(SIGUSR2, &sa_SIGUSR2, NULL);

  // SIGIO
  struct sigaction sa_SIGIO;
  memset(&sa_SIGIO, 0, sizeof(sa_SIGIO));
  sa_SIGIO.sa_sigaction = sigio_handler;
  sa_SIGIO.sa_flags = SA_SIGINFO;
  ret_value = sigaction(SIGIO, &sa_SIGIO, NULL);

  return ret_value;
}

static void sigusr1_handler(int sig_num, siginfo_t* si, void* ignored) {
  switch (si->si_code)
  {
  case SI_USER: // signal received via kill()
  {
    int found_idx = -1;
    for (int i = 0; i < MAX_TCP_CONNECTIONS; i++)
    {
      if (connections[i].child_pid == si->si_pid)
      {
        timing_arr[i] = 0;
        found_idx = i;
        break;
      }

    }

    char* buffer = NULL;

    header_t header;

    read(connections[found_idx].child_pipe, &header, sizeof(header));
    close(connections[found_idx].child_pipe);

    asprintf(&buffer, "IP: '%s' Lifetime: '%d' Filetype: '%s' Filesize: '%ld' Blur: %d Upscale: %d Downscale: %d PID: '%d' TT: '%ld'\n", 
      connections[found_idx].ip_addr, connections[found_idx].lifetime,
      header.file_extension, header.file_size, header.blur_multiplier, header.upscale_multiplier, header.downscale_multiplier,
      connections[found_idx].child_pid, connections[found_idx].timing_thread
    );

    int fd =  open("server_log.txt", O_RDWR | O_CREAT | O_APPEND, S_IWUSR | S_IWGRP | S_IWOTH | S_IRUSR | S_IRGRP | S_IROTH);

    write(fd, buffer, strlen(buffer));

    close(fd);

    connections[found_idx].child_pid = 0;
    connections[found_idx].child_pipe = 0;
    memset(connections[found_idx].ip_addr, 0, sizeof("255.255.255.255"));
    connections[found_idx].lifetime = 0;
    connections[found_idx].timing_thread = 0;

    printf("Child[%d]: Finished execution\n", si->si_pid);
    fflush(stdout);
    break;
  }

  default:
  {
    printf("ERROR PROCESSING SINGAL: %d", sig_num);
    break;
  }
  }
  interrupted_by_sig_handler = true;
}

static void sigusr2_handler(int sig_num, siginfo_t* si, void* ignored) {
  switch (si->si_code)
  {
  case SI_USER: // signal received via kill()
  {
    printf("Child[%d]: Admin disconnected\n", si->si_pid);
    fflush(stdout);
    break;
  }

  default:
  {
    printf("ERROR PROCESSING SINGAL: %d", sig_num);
    break;
  }
  }
  interrupted_by_sig_handler = true;
  admin_connected = false;
}

static void sigio_handler(int sig_num, siginfo_t* si, void* ignored) {
  switch (si->si_code)
  {
  case SI_USER: // signal received via kill()
  {
    int count = 0;
    for (int i = 0; i < MAX_TCP_CONNECTIONS; i++)
    {
      if (connections[i].child_pid != 0)
      {
        count++;
      }
    }

    write(udp_handler_pipe, &(count), sizeof(int));

    for (int i = 0; i < MAX_TCP_CONNECTIONS; i++)
    {
      if (connections[i].child_pid != 0)
      {
        write(udp_handler_pipe, &(connections[i]), sizeof(tcp_connection_t));
      }
    }

    break;
  }

  default:
  {
    printf("ERROR PROCESSING SINGAL: %d", sig_num);
    break;
  }
  }
  interrupted_by_sig_handler = true;
}