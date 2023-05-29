#include "sig_handler.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool interrupted_by_sig_handler = false;

static void sigusr1_handler(int sig_num, siginfo_t* si, void* ignored);

int init_sig_handlers()
{
  int ret_value = -1;

  // SIGUSR1
  struct sigaction sa_SIGUSR1;
  memset(&sa_SIGUSR1, 0, sizeof(sa_SIGUSR1));
  sa_SIGUSR1.sa_sigaction = sigusr1_handler;
  sa_SIGUSR1.sa_flags = SA_SIGINFO;
  ret_value = sigaction(SIGUSR1, &sa_SIGUSR1, NULL);

  return ret_value;
}

static void sigusr1_handler(int sig_num, siginfo_t* si, void* ignored) {
  switch (si->si_code)
  {
  case SI_USER: // signal received via kill()
  {
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