
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/wait.h>

#include <netinet/in.h>

#include "common_types.h"

#define SECRET_TOKEN "DEADBEEF5A5A5A5A"

bool admin_connected = false;
int udp_handler_pipe = -1;

pid_t udp_handler(int server_socket_fd_udp)
{

  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(client_addr));
  socklen_t len = sizeof(client_addr);

  char buffer[MAX_LINE_SIZE];
  memset(buffer, 0, sizeof(buffer));

  int n = recvfrom(server_socket_fd_udp, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &len);

  //user password
  char* user = strtok(buffer, " ");
  char* password = strtok(NULL, "\0");

  if ( !((strncmp(user, "admin", 5) == 0) && (strncmp(password, "admin", 5) == 0))  )
  {
    sendto(server_socket_fd_udp, "Wrong credentials", 18, 0, (struct sockaddr*)&client_addr, len);
    return -1;
  }

  sendto(server_socket_fd_udp, SECRET_TOKEN, sizeof(SECRET_TOKEN), 0, (struct sockaddr*)&client_addr, len);

  int pipe_fd[2];
  if (pipe(pipe_fd) == -1)
  {
    printf("error creating pipes\n");
    exit(EXIT_FAILURE);
  }

  memset(buffer, 0, sizeof(buffer));

  pid_t child_pid = fork();
  if (child_pid == 0)
  {
    printf("Child[%d]: Admin connected\n", getpid());
    close(pipe_fd[1]);
    udp_handler_pipe = pipe_fd[0];

    pid_t parent_pid = getppid();

    while (true)
    {
      int n = recvfrom(server_socket_fd_udp, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &len);
      
      char* token = strtok(buffer, " ");
      char* contents = strtok(NULL, "\0");

      if (strncmp(token, SECRET_TOKEN, sizeof(SECRET_TOKEN)) != 0)
      {
        sendto(server_socket_fd_udp, "Occupied", 9u, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        continue;
      }

      if (strncmp("hist", contents, 4) == 0)
      {
        int fd = open("server_log.txt", O_RDONLY | O_CREAT);

        struct stat st;
        stat("server_log.txt", &st);
        char* file_buffer = (char*)malloc(st.st_size);

        read(fd, file_buffer, st.st_size);


        sendto(server_socket_fd_udp, &st.st_size, sizeof(st.st_size), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        sendto(server_socket_fd_udp, file_buffer, st.st_size, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));

        close(fd);
      }

      if (strncmp("clear logs", contents, strlen(contents)-1) == 0)
      {
        int fd = open("server_log.txt", O_RDONLY | O_CREAT | O_TRUNC);
        close(fd);
      }

      if (strncmp("clear files", contents, strlen(contents)-1) == 0)
      {
        system("rm -f server/*");
      }

      if (strncmp("conn", contents, strlen(contents)-1) == 0)
      {
        kill(getppid(), SIGIO);

        char* conn_buff = NULL;

        int conn_count = 0;
        read(udp_handler_pipe, &conn_count, sizeof(int));

        sendto(server_socket_fd_udp, &conn_count, sizeof(int), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));

        for (int i = 0; i < conn_count; i++)
        {
          tcp_connection_t conn;
          read(udp_handler_pipe, &conn, sizeof(tcp_connection_t));

          asprintf(&conn_buff, "IP: '%s' Lifetime: '%d' PID: '%d' TT: '%ld'\n", 
            conn.ip_addr, conn.lifetime,
            conn.child_pid, conn.timing_thread
          );

          sendto(server_socket_fd_udp, conn_buff, strlen(conn_buff), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));

          conn_buff =NULL;
        }
      }

      if (strncmp("kick", contents, 4) == 0)
      {
        char* pid_str = strtok(contents, " ");
        pid_str = strtok(NULL, "\n");

        int pid = strtol(pid_str, NULL, 10);

        kill(pid, SIGUSR1);
      }

      if (strncmp("ban", contents, 3) == 0)
      {
        char* banned_ip = strtok(contents, " ");
        banned_ip = strtok(NULL, "\n");

        int fd = open("banned_ips.txt", O_WRONLY | O_APPEND);

        write(fd, banned_ip, strlen(banned_ip));
        write(fd, "\n", 1);

        close(fd);
      }

      if (strncmp("exit", contents, 4) == 0)
      {
        admin_connected = false;

        break;
      }

      memset(buffer, 0, sizeof(buffer));
    }

    // send signal that child finished processing request
    kill(parent_pid, SIGUSR2);

    exit(EXIT_SUCCESS);
  }

  close(pipe_fd[0]);
  udp_handler_pipe = pipe_fd[1];

  return child_pid;

}