#include "tcp_handler.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>

#include <netinet/in.h>

pid_t tcp_create_handler(int server_socket_fd_tcp)
{

  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(client_addr));

  socklen_t len = 0;

  char buffer[MAX_LINE_SIZE];
  memset(buffer, 0, sizeof(buffer));

  int client_fd = accept(server_socket_fd_tcp, (struct sockaddr*)&client_addr, &len);

  pid_t child_pid = fork();

  if (child_pid == 0)
  {
    // child
    pid_t parent_pid = getppid();

    close(server_socket_fd_tcp);

    while(true)
    {
      memset(buffer, 0, sizeof(buffer));

      int recv_size = recv(client_fd, buffer, sizeof(buffer), 0);

      if (recv_size <= 0 || strncmp(buffer, "close", 5) == 0)
      {
        break;
      }

      printf("Child[%d]: %s", getpid(), buffer);

      send(client_fd, "Hello from server TCP!\n", 24, 0);
    }

    close(client_fd);

    // send signal that child finished processing request
    kill(parent_pid, SIGUSR1);

    exit(EXIT_SUCCESS);
  }

  close(client_fd);

  return child_pid;

}
