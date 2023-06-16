#pragma once

#include <sys/types.h>

pid_t udp_handler(int server_socket_fd_udp);

extern bool admin_connected;

extern int udp_handler_pipe;