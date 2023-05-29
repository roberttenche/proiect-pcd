#pragma once

#include <sys/types.h>

// creates child process that handles a TCP request
// returns: fork() output
pid_t tcp_create_handler(int server_socket_fd_udp);
