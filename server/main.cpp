#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <resolv.h>

#include <fcntl.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 42070
int max(int x, int y)
{
  if (x > y) return x;
  else       return y;
}

extern char** environ;

int main(int argc, char* argv[], char* envp[])
{
  int server_socket_fd_tcp, server_socket_fd_udp;
  int client_socket_fd, client_len;
  int max_fd;

  fd_set file_descriptor_set;

  struct sockaddr_in server_addr, client_addr;

  server_socket_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);

  server_socket_fd_udp = socket(AF_INET, SOCK_DGRAM, 0);

  if (server_socket_fd_tcp < 0 || server_socket_fd_udp < 0)
  {
    printf("error creating sockets\n");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0u, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  server_addr.sin_port = htons(SERVER_PORT);

  if (
    bind(server_socket_fd_tcp, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ||
    bind(server_socket_fd_udp, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0
    )
  {
    printf("error binding sockets\n");
    exit(EXIT_FAILURE);
  }

  printf("Server started: %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

  if (listen(server_socket_fd_tcp, 1024) < 0)
  {
    printf("error listening sockets\n");
    exit(EXIT_FAILURE);
  }

  memset(&file_descriptor_set, 0, sizeof(fd_set));

  max_fd = max(server_socket_fd_tcp, server_socket_fd_udp) + 1;

  while(true)
  {
    FD_SET(server_socket_fd_tcp, &file_descriptor_set);
    FD_SET(server_socket_fd_udp, &file_descriptor_set);

    select(max_fd, &file_descriptor_set, NULL, NULL, NULL);

    if(FD_ISSET(server_socket_fd_tcp, &file_descriptor_set))
    {
      socklen_t len = 0;
      char buffer[200];
      memset(buffer, 0, sizeof(buffer));

      int client_fd = accept(server_socket_fd_tcp, (struct sockaddr*)&client_addr, &len);
      pid_t child_pid;

      printf("Message From TCP client: ");
      read(client_fd, buffer, sizeof(buffer));

      puts(buffer);

      write(client_fd, "Hello from server TCP!\n", 24);

      close(client_fd);
    }

    if(FD_ISSET(server_socket_fd_udp, &file_descriptor_set))
    {
      socklen_t len = 0;
      char buffer[200];
      memset(buffer, 0, sizeof(buffer));

      len = sizeof(client_addr);

      printf("Message from UDP client: ");
      int n = recvfrom(server_socket_fd_udp, buffer, sizeof(buffer), 0,
                  (struct sockaddr*)&client_addr, &len);

      printf("%s", buffer);

      sendto(server_socket_fd_udp, "Hello from server UDP!\n", 24u, 0,
          (struct sockaddr*)&client_addr, sizeof(client_addr));
    }

  }

  return 0;
}