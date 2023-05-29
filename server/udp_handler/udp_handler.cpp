#include "udp_handler.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>

void udp_handle(int server_socket_fd_udp)
{

  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(client_addr));

  socklen_t len = 0;

  char buffer[MAX_LINE_SIZE];
  memset(buffer, 0, sizeof(buffer));

  len = sizeof(client_addr);

  printf("Message from UDP client: ");
  int n = recvfrom(server_socket_fd_udp, buffer, sizeof(buffer), 0,
              (struct sockaddr*)&client_addr, &len);

  printf("%s", buffer);

  sendto(server_socket_fd_udp, "Hello from server UDP!\n", 24u, 0,
      (struct sockaddr*)&client_addr, sizeof(client_addr));

  fflush(stdout);
}