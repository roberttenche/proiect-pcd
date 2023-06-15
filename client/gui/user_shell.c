#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>

#include <pthread.h>

void flushInputBuffer() {
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

int user_shell()
{
  flushInputBuffer();

  int file_idx = -1;
  file_t files[50];

  memset(files, 0, sizeof(files));

  system("clear");
  printf("User mode\n");

  while (1)
  {
    int valid_command = 0;
    char command[100];

    printf(">>> ");
    fgets(command, sizeof(command), stdin);

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
      return 0;
    }

    if (command != NULL && file_idx == -1 && valid_command == 1)
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