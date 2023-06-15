#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "user_shell.h"

int main()
{
  system("clear");
  int choice;

  while (1) {
    printf("Main Menu\n");
    printf("1. User\n");
    printf("2. Admin\n");
    printf("2. Exit\n");
    printf("Enter your choice (1-3): ");
    scanf("%d", &choice);

    switch (choice) {
      case 1:
        user_shell();
        break;

      case 2:
        printf("You selected Admin\n");
        // Add your code for Option 2 here
        break;

      case 3:
        printf("Have a nice day!\n");
        exit(EXIT_SUCCESS);
        break;

      default:
        printf("Invalid choice\n");
    }
    system("clear");
  }

}