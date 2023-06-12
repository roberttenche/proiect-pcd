#include "arg_parser.h"
#include "common_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/stat.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <resolv.h>

#include <fcntl.h>
#include <pthread.h>

char* getFileExtension(const char* fileName) {
    char* extension = strrchr(fileName, '.');
    if (extension == NULL) {
      return "";  // No extension found
    } else {
      return extension + 1;  // Exclude the dot (.)
    }
}

int main(int argc, char* argv[], char* envp[])
{

  char* file_path = NULL;
  int apply_blur = 0;
  int apply_upscale = 0;
  int apply_downscale = 0;

  arg_parse(argc, argv, envp, &file_path, &apply_blur, &apply_upscale, &apply_downscale);

  if (file_path == NULL)
  {
    printf("[ERROR]: No input file specified. Use -f or --file.\n");
    exit(EXIT_FAILURE);
  }

  // open file with only reading permission
  int file_fd = open(file_path, O_RDONLY);

  if (file_fd < 0)
  {
    printf("[ERROR]: File not accesible: %s\n", file_path);
    exit(EXIT_FAILURE);
  }

  int client_socket_fd;
  struct sockaddr_in server_addr;

  client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (client_socket_fd < 0)
  {
    printf("[ERROR] Error creating socket\n");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0u, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  server_addr.sin_port = htons(SERVER_PORT);

  if (connect(client_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("[ERROR] Error creating socket\n");
    exit(EXIT_FAILURE);
  }

  struct stat fileStat;

  if (fstat(file_fd, &fileStat) == -1) {
    printf("[ERROR] Error while getting file info.\n");
    exit(EXIT_FAILURE);
  }

  header_t header;
  memset(&header, 0, sizeof(header));

  strncpy(header.file_extension, getFileExtension(file_path), sizeof(header.file_extension));
  header.file_size = fileStat.st_size;
  
  if (apply_blur == 1)
  {
    header.processing_type |= PROCESSING_TYPE_BLUR;
  }
  if (apply_upscale == 1)
  {
    header.processing_type |= PROCESSING_TYPE_UPSCALE;
  }
  if (apply_downscale == 1)
  {
    header.processing_type |= PROCESSING_TYPE_DOWNSCALE;
  }


  char buffer[MAX_LINE_SIZE + 1];
  memset(buffer, 0, sizeof(buffer));

  char response[3];
  memset(response, 0, sizeof(response));

  send(client_socket_fd, &header, sizeof(header), 0u);

  size_t read_count = 0;
  while (read_count = read(file_fd, buffer, MAX_LINE_SIZE))
  {

RESEND:
    send(client_socket_fd, buffer, read_count, 0u);

    recv(client_socket_fd, response, MAX_LINE_SIZE, 0);

    if (strncmp(response, "OK", sizeof(response)) != 0)
    {
      printf("[ERROR] Error while sending packet\n");
      goto RESEND;
    }

    memset(buffer, 0, sizeof(buffer));
  }

  close(client_socket_fd);
  close(file_fd);

  exit(EXIT_SUCCESS);
}
