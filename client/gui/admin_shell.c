#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include <pthread.h>

static void fflush_stdin() {
  char c;
  while ((c = getchar()) != '\n' && c != EOF) {}
}

extern char** environ;

typedef struct file_s
{
  char* path;
  unsigned char blur_multiplier;
  unsigned char upscale_multiplier;
  unsigned char downscale_multiplier;
} file_t;

int client_socket_fd;
char token[18];
struct sockaddr_in server_addr;
socklen_t len;


static void unexpected_exit_handler(int sig_num, siginfo_t* si, void* ignored) {

  char* auth_commmand = NULL;
  asprintf(&auth_commmand, "%s %s", token, "exit");

  sendto(client_socket_fd, auth_commmand, strlen(auth_commmand), 0, (struct sockaddr*)&server_addr, len);

  exit(EXIT_SUCCESS);

}


int admin_shell()
{
  struct sigaction sa_UNEXPECTED_EXIT;
  memset(&sa_UNEXPECTED_EXIT, 0, sizeof(sa_UNEXPECTED_EXIT));
  sa_UNEXPECTED_EXIT.sa_sigaction = unexpected_exit_handler;
  sa_UNEXPECTED_EXIT.sa_flags = SA_SIGINFO;
  sigaction(SIGINT, &sa_UNEXPECTED_EXIT, NULL);
  sigaction(SIGHUP, &sa_UNEXPECTED_EXIT, NULL);
  sigaction(SIGTSTP, &sa_UNEXPECTED_EXIT, NULL);


  fflush_stdin();

  int file_idx = -1;
  file_t files[50];

  memset(files, 0, sizeof(files));

  system("clear");
  printf("Admin mode\n");

  char username[25];
  char password[25];

  printf("Username: ");
  fgets(username, sizeof(username), stdin);

  printf("Password: ");
  fgets(password, sizeof(password), stdin);

  if (strlen(username) <= 1 || strlen(password) <= 1)
  {
    return -1;
  }

  memset(&server_addr, 0u, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  server_addr.sin_port = htons(SERVER_PORT);
  len = sizeof(server_addr);

  client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

  if (connect(client_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("error creating socket\n");
    sleep(3);
    return -1;
  }

  char* auth_str = NULL;
  asprintf(&auth_str, "%s %s", username, password);

  sendto(client_socket_fd, auth_str, strlen(auth_str), 0, (struct sockaddr*)&server_addr, len);

  int read_count = recvfrom(client_socket_fd, token, sizeof(token), 0, (struct sockaddr*)&server_addr, &len);

  if (strncmp("Occupied", token, 9) == 0)
  {
    printf("Admin already connected\n");
    sleep(3);
    return 1;
  }
  else if (strncmp("Wrong credentials", token, 18) == 0)
  {
    printf("Wrong credentials\n");
    sleep(3);
    return 1;
  }
  else if (read_count <= -1)
  {
    printf("Failed connection to server\n");
    sleep(3);
    return 1;
  }
  

  while (1)
  {
    int valid_command = 0;
    int admin_command = 0;
    char command[100];

    printf(">>> ");
    fgets(command, sizeof(command), stdin);

    /// HIST
    if (strncmp("hist", command, 4) == 0)
    {

      char* auth_commmand = NULL;
      asprintf(&auth_commmand, "%s %s", token, command);

      sendto(client_socket_fd, auth_commmand, strlen(auth_commmand), 0, (struct sockaddr*)&server_addr, len);


      size_t log_size = 0;
      recvfrom(client_socket_fd, &log_size, sizeof(size_t), 0, (struct sockaddr*)&server_addr, &len);

      char* file_buffer = malloc(log_size);
      recvfrom(client_socket_fd, file_buffer, log_size, 0, (struct sockaddr*)&server_addr, &len);

      printf("%s\n", file_buffer);

      admin_command = 1;
      valid_command = 1;
    }

    /// CLEAR
    if (strncmp("clear", command, strlen(command)-1) == 0)
    {

      system("clear");

      admin_command = 1;
      valid_command = 1;
      continue;
    }

    /// CLEAR LOGS
    if (strncmp("clear logs", command, strlen(command)-1) == 0)
    {

      char* auth_commmand = NULL;
      asprintf(&auth_commmand, "%s %s", token, command);

      sendto(client_socket_fd, auth_commmand, strlen(auth_commmand), 0, (struct sockaddr*)&server_addr, len);

      admin_command = 1;
      valid_command = 1;
      continue;
    }

    /// CLEAR FILES
    if (strncmp("clear files", command, strlen(command)-1) == 0)
    {

      char* auth_commmand = NULL;
      asprintf(&auth_commmand, "%s %s", token, command);

      sendto(client_socket_fd, auth_commmand, strlen(auth_commmand), 0, (struct sockaddr*)&server_addr, len);

      admin_command = 1;
      valid_command = 1;
      continue;
    }

    /// CONN
    if (strncmp("conn", command, strlen(command)-1) == 0)
    {
      char* auth_commmand = NULL;
      asprintf(&auth_commmand, "%s %s", token, command);

      sendto(client_socket_fd, auth_commmand, strlen(auth_commmand), 0, (struct sockaddr*)&server_addr, len);

      int conn_count = 0;
      recvfrom(client_socket_fd, &conn_count, sizeof(int), 0, (struct sockaddr*)&server_addr, &len);

      for (int i = 0; i < conn_count; i++)
      {
        char conn_buffer[MAX_LINE_SIZE];
        recvfrom(client_socket_fd, conn_buffer, MAX_LINE_SIZE, 0, (struct sockaddr*)&server_addr, &len);
        printf("%s\n", conn_buffer);
      }

      admin_command = 1;
      valid_command = 1;
      continue;
    }

    /// KICK
    if (strncmp("kick", command, 4) == 0)
    {
      char* auth_commmand = NULL;
      asprintf(&auth_commmand, "%s %s", token, command);

      sendto(client_socket_fd, auth_commmand, strlen(auth_commmand), 0, (struct sockaddr*)&server_addr, len);

      admin_command = 1;
      valid_command = 1;
      continue;
    }

    /// BAN
    if (strncmp("ban", command, 3) == 0)
    {
      char* auth_commmand = NULL;
      asprintf(&auth_commmand, "%s %s", token, command);

      sendto(client_socket_fd, auth_commmand, strlen(auth_commmand), 0, (struct sockaddr*)&server_addr, len);

      admin_command = 1;
      valid_command = 1;
      continue;
    }

    /// FILE
    if (strncmp("file", command, 4) == 0)
    {
      file_idx++;

      char* file_path = strtok(command, " ");
      file_path = strtok(NULL, "\n");

      files[file_idx].path = malloc(strlen(file_path));

      strcpy(files[file_idx].path, file_path);

      valid_command = 1;
    }

    /// BLUR
    if (strncmp("blur", command, 4) == 0 && file_idx != -1)
    {
      char* blur_amount = strtok(command, " ");
      blur_amount = strtok(NULL, "\n");

      files[file_idx].blur_multiplier = strtol(blur_amount, NULL, 10);

      valid_command = 1;
    }
    else if (strncmp("blur", command, 4) == 0)
    {
      valid_command = 1;
    }
    

    /// UPSCALE
    if (strncmp("upscale", command, 7) == 0 && file_idx != -1)
    {
      char* upscale_amount = strtok(command, " ");
      upscale_amount = strtok(NULL, "\n");

      files[file_idx].upscale_multiplier = strtol(upscale_amount, NULL, 10);

      valid_command = 1;
    }
    else if (strncmp("upscale", command, 7) == 0)
    {
      valid_command = 1;
    }

    /// DOWNSCALE
    if (strncmp("downscale", command, 9) == 0 && file_idx != -1)
    {
      char* downscale_amount = strtok(command, " ");
      downscale_amount = strtok(NULL, "\n");

      files[file_idx].downscale_multiplier = strtol(downscale_amount, NULL, 10);

      valid_command = 1;
    }
    else if (strncmp("downscale", command, 9) == 0)
    {
      valid_command = 1;
    }

    /// LIST
    if (strncmp("list", command, 4) == 0 && file_idx != -1)
    {
      for (int i = 0; i <= file_idx; i++)
      {
        printf("file: '%s' blur: '%d' upscale: '%d' downscale: '%d'\n", 
          files[i].path, 
          files[i].blur_multiplier, 
          files[i].upscale_multiplier, 
          files[i].downscale_multiplier
        );
      }
      valid_command = 1;
    }
    else if (strncmp("list", command, 4) == 0)
    {
      valid_command = 1;
    }

    /// EXECUTE
    if (strncmp("execute", command, 7) == 0 && file_idx != -1)
    {
      for (int i = 0; i <= file_idx; i++)
      {
        pid_t pid = fork();

        if (pid <= -1)
        {
          printf("execute error\n");
        }
        else if (pid == 0)
        {
          char blur_str[20];
          char upscale_str[20];
          char downscale_str[20];

          sprintf(blur_str, "%d", files[i].blur_multiplier);
          sprintf(upscale_str, "%d", files[i].upscale_multiplier);
          sprintf(downscale_str, "%d", files[i].downscale_multiplier);

          execle("user.app", "user.app",
            "--file", files[i].path,
            "--blur", blur_str,
            "--upscale", upscale_str,
            "--downscale", downscale_str,

            NULL,
            environ);

          exit(EXIT_FAILURE); // command cannot be run
        }
        else
        {
          wait(NULL);
        }
      };
      valid_command = 1;
    }
    else if (strncmp("execute", command, 7) == 0)
    {
      valid_command = 1;
    }

    /// EXIT
    if (strncmp("exit", command, 4) == 0)
    {

      char* auth_commmand = NULL;
      asprintf(&auth_commmand, "%s %s", token, command);

      sendto(client_socket_fd, auth_commmand, strlen(auth_commmand), 0, (struct sockaddr*)&server_addr, len);

      return 0;
    }

    if (command != NULL && file_idx == -1 && valid_command == 1 && admin_command == 0)
    {
      printf("Use 'file' command first.\n");
    }
    else if (command != NULL && valid_command == 0)
    {
      command[strlen(command)-1] = '\0';
      printf("Unrecognized command: '%s'\n", command);
    }

  }

  return -1;

}