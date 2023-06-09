#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "sig_handler.hpp"
#include "tcp_handler.hpp"
#include "udp_handler.hpp"

#include "common_types.h"
#include "my_timer.hpp"

#include "pthread.h"

int max(int x, int y) { if (x > y) return x; else return y; }

extern char** environ;

tcp_connection_t* connections;
int current_connection_idx = 0;

int main(int argc, char* argv[], char* envp[])
{

  system("mkdir -p server/");
  system("touch banned_ips.txt");

  if (init_sig_handlers() == -1)
  {
    printf("error initializing signal handlers\n");
    exit(EXIT_FAILURE);
  }

  connections = (tcp_connection_t*)malloc(MAX_TCP_CONNECTIONS*(sizeof(tcp_connection_t)));
  for (int i = 0; i < MAX_TCP_CONNECTIONS; i++)
  {
    connections[i].child_pid = 0;
    connections[i].child_pipe = 0;
    memset(connections[i].ip_addr, 0, sizeof("255.255.255.255"));
    connections[i].lifetime = 0;
    connections[i].timing_thread = 0;
  }

  int server_socket_fd_tcp, server_socket_fd_udp;
  int client_socket_fd, client_len;
  int max_fd;

  // struct timeval timeout_tcp;
  // timeout_tcp.tv_sec = 0xFFFFFFFF; // seconds
  // timeout_tcp.tv_usec = 0; // microseconds

  fd_set file_descriptor_set;
  memset(&file_descriptor_set, 0, sizeof(fd_set));

  struct sockaddr_in server_addr;

  server_socket_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);

  server_socket_fd_udp = socket(AF_INET, SOCK_DGRAM, 0);

  if (server_socket_fd_tcp < 0 || server_socket_fd_udp < 0)
  {
    printf("error creating sockets\n");
    exit(EXIT_FAILURE);
  }

  // if (setsockopt(server_socket_fd_tcp, SOL_SOCKET, SO_RCVTIMEO, &timeout_tcp, sizeof(timeout_tcp)) < 0) {
  //   printf("error adding tcp socket options\n");
  //   exit(EXIT_FAILURE);
  // }

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

  printf("Server started: %s:%d PID[%d]\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), getpid());

  if (listen(server_socket_fd_tcp, MAX_TCP_CONNECTIONS) < 0)
  {
    printf("error listening sockets\n");
    exit(EXIT_FAILURE);
  }

  max_fd = max(server_socket_fd_tcp, server_socket_fd_udp) + 1;

  while(true)
  {
    FD_SET(server_socket_fd_tcp, &file_descriptor_set);
    FD_SET(server_socket_fd_udp, &file_descriptor_set);

    while (interrupted_by_sig_handler == false)
    {
      select(max_fd, &file_descriptor_set, NULL, NULL, NULL);
      interrupted_by_sig_handler = !interrupted_by_sig_handler;
    }

    if(FD_ISSET(server_socket_fd_tcp, &file_descriptor_set))
    {
      pid_t child_pid = -1;

      if ( (child_pid = tcp_create_handler(server_socket_fd_tcp, current_connection_idx)) == -1)
      {
        printf("Error creating TCP handler!\n");
        exit(EXIT_FAILURE);
      }
      else if (child_pid == -2)
      {
        printf("Encounctered blocked IP\n");
      }
      else if (child_pid >= 1)
      {
        pthread_t thread;

        int temp = current_connection_idx;
        pthread_create(&thread, NULL, my_timer, (void*)(&temp));

        connections[current_connection_idx].child_pid = child_pid;
        connections[current_connection_idx].timing_thread = thread;

        current_connection_idx++;
      }
      else
      {
        printf("Error creating TCP handler!\n");
        exit(EXIT_FAILURE);
      }

    }

    if(FD_ISSET(server_socket_fd_udp, &file_descriptor_set) && admin_connected == false)
    {
      if (udp_handler(server_socket_fd_udp) > 0)
      {
        admin_connected = true;
      }

    }

    interrupted_by_sig_handler = false;

  }

  return 0;
}