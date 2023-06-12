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
#include "blur.hpp"


pid_t tcp_create_handler(int server_socket_fd_tcp)
{

  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(client_addr));

  socklen_t len = 0;

  char buffer[MAX_LINE_SIZE + 1];
  memset(buffer, 0, sizeof(buffer));

  int client_fd = accept(server_socket_fd_tcp, (struct sockaddr*)&client_addr, &len);

  pid_t child_pid = fork();

  if (child_pid == 0)
  {
    close(server_socket_fd_tcp);

    char client_ip[INET_ADDRSTRLEN];

    if (inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN) == NULL) {
      printf("Child[%d]: Error converting IP address to string.\n", getpid());
      exit(EXIT_FAILURE);
    }

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

    asprintf(&file_path, "./server/%d.%s",(int)time(NULL), header.file_extension);

    while(true)
    {
      memset(buffer, 0, sizeof(buffer));

      int recv_size = recv(client_fd, buffer, MAX_LINE_SIZE + 1, 0);

      if (recv_size <= 0 || strncmp(buffer, "close", 5) == 0)
      {

        if (total_size_received == header.file_size)
        {
          printf("Child[%d]: File received successfully.\n", getpid());
        }
        else
        {
          printf("Child[%d]: Error recv -1\n", getpid());
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

      send(client_fd, "OK", sizeof("OK"), 0);

    }

    if ((header.processing_type & PROCESSING_TYPE_BLUR) != 0)
    {
      char path[PATH_MAX];
      char* absolutePath = realpath(file_path, path);
      printf("%s\n", absolutePath);
      apply_blur(absolutePath);
    }
    if ((header.processing_type & PROCESSING_TYPE_UPSCALE) != 0)
    {
      printf("upscale\n");
    }
    if ((header.processing_type & PROCESSING_TYPE_DOWNSCALE) != 0)
    {
      printf("downscale\n");
    }



    close(client_fd);

    // send signal that child finished processing request
    kill(parent_pid, SIGUSR1);

    exit(EXIT_SUCCESS);
  }

  close(client_fd);

  return child_pid;

}
