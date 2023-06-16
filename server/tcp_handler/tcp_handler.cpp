#include "tcp_handler.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <sys/wait.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <time.h>
#include <limits.h>

#include "common_types.h"
#include "processing.hpp"

extern tcp_connection_t* connections;

static void sigusr1_handler(int sig_num, siginfo_t* si, void* ignored);

pid_t tcp_create_handler(int server_socket_fd_tcp, int current_connection_idx)
{
  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(client_addr));

  socklen_t len = 0;

  char buffer[MAX_LINE_SIZE + 1];
  memset(buffer, 0, sizeof(buffer));

  int client_fd = accept(server_socket_fd_tcp, (struct sockaddr*)&client_addr, &len);

  char client_ip[INET_ADDRSTRLEN];

  int pipe_fd[2];
  if (pipe(pipe_fd) == -1)
  {
    printf("error creating pipes\n");
    exit(EXIT_FAILURE);
  }

  if (inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN) == NULL)
  {
    printf("Child[%d]: Error converting IP address to string.\n", getpid());
    exit(EXIT_FAILURE);
  }

  printf("Incoming connection from IP: %s\n", client_ip);

  int fd = open("banned_ips.txt", O_RDONLY);

  char blocked_ip[15];
  memset(blocked_ip, 0, sizeof(blocked_ip));

  char c;
  int idx = 0;
  while (read(fd, &c, 1) == 1)
  {
    if (c == '\n')
    {
      if (strncmp(blocked_ip, client_ip, 15) == 0)
      {
        close(client_fd);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        close(fd);
        return -2;
      }
      memset(blocked_ip, 0, sizeof(blocked_ip));
      idx = 0;
    }
    else
    {
      blocked_ip[idx++] = c;
    }
  }
  

  close(fd);


  strcpy(connections[current_connection_idx].ip_addr, client_ip);

  pid_t child_pid = fork();

  if (child_pid == 0)
  {
    // SIGUSR1
    struct sigaction sa_SIGUSR1;
    memset(&sa_SIGUSR1, 0, sizeof(sa_SIGUSR1));
    sa_SIGUSR1.sa_sigaction = sigusr1_handler;
    sa_SIGUSR1.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa_SIGUSR1, NULL);
    
    close(pipe_fd[0]);

    close(server_socket_fd_tcp);

    printf("Child[%d]: Started processing connection from ip: %s\n", getpid(), client_ip);

    // child
    pid_t parent_pid = getppid();

    char* file_path = NULL;
    size_t total_size_received = 0;

    header_t header;
    memset(&header, 0, sizeof(header));

    if (recv(client_fd, &header, sizeof(header), 0) <= 0)
    {
      printf("Child[%d]: Error while getting header\n", getpid());
      close(client_fd);
      exit(EXIT_FAILURE);
    }

    int unix_timestamp = (int)time(NULL);

    write(pipe_fd[1], &header, sizeof(header_t));

    asprintf(&file_path, "./server/%d.%s", unix_timestamp, header.file_extension);

    while(true)
    {
      memset(buffer, 0, sizeof(buffer));

      int recv_size = recv(client_fd, buffer, MAX_LINE_SIZE + 1, 0);

      if (recv_size <= 0 || strncmp(buffer, "CL_TR_DONE", 10) == 0)
      {

        if (total_size_received == header.file_size)
        {
          printf("Child[%d]: File received successfully.\n", getpid());
        }
        else
        {
          printf("Child[%d]: Error recv\n", getpid());
          close(client_fd);

          // send signal that child finished processing request
          kill(parent_pid, SIGUSR1);

          exit(EXIT_FAILURE);
        }
        break;
      }

      int fd =  open(file_path, O_RDWR | O_CREAT | O_APPEND, S_IWUSR | S_IWGRP | S_IWOTH | S_IRUSR | S_IRGRP | S_IROTH);

      write(fd, buffer, recv_size);

      total_size_received += recv_size;

      close(fd);

      send(client_fd, "SV_OK", sizeof("SV_OK"), 0);

    }

    char path[PATH_MAX];
    char* absolutePath = realpath(file_path, path);
    apply_processing(absolutePath, header, unix_timestamp);

    char* output = NULL;
    asprintf(&output, "./server/%d_output.mp4", unix_timestamp);

    int file_fd = open(output, O_RDONLY);

    memset(buffer, 0, sizeof(buffer));

    char response[6];
    memset(response, 0, sizeof(response));

    size_t read_count = 0;
    while (read_count = read(file_fd, buffer, MAX_LINE_SIZE))
    {

    RESEND:
      send(client_fd, buffer, read_count, 0u);

      recv(client_fd, response, MAX_LINE_SIZE, 0);

      if (strncmp(response, "CL_OK", sizeof(response)) != 0)
      {
        printf("[ERROR] Error while sending packet\n");
        goto RESEND;
      }

      memset(buffer, 0, sizeof(buffer));
    }

    close(client_fd);

    // send signal that child finished processing request
    kill(parent_pid, SIGUSR1);

    exit(EXIT_SUCCESS);
  }

  close(pipe_fd[1]);

  connections[current_connection_idx].child_pipe = pipe_fd[0];

  close(client_fd);

  return child_pid;

}

static void sigusr1_handler(int sig_num, siginfo_t* si, void* ignored)
{
  switch (si->si_code)
  {
  case SI_USER: // signal received via kill()
  {
    kill(getppid(), SIGUSR1);
    kill(getpid(), SIGKILL);
    break;
  }

  default:
  {
    printf("ERROR PROCESSING SINGAL: %d", sig_num);
    break;
  }
  }
}
