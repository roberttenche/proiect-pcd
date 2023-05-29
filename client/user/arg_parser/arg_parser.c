#include "arg_parser.h"

#include <stdio.h>  /* for printf */
#include <stdlib.h> /* for exit */
#include <getopt.h> /* for getopt */

static struct option opt_long[] = {
  {"help",      no_argument,       0, 0},
  {"processes",  required_argument, 0, 0},
};

static void print_help();

extern char** environ;

void arg_parse(int argc, char* argv[], char* envp[])
{
  int opt = 0;
  int opt_idx = 0;


  /* read all options and arguments form the command line*/
  while ((opt = getopt_long(argc, argv, "hp:", opt_long, &opt_idx)) != -1)
  {
    switch (opt)
    {
      case 0:
      {
        /* Long options */

        /* --help */
        if (opt_idx == 0)
        {
          print_help();
          exit(EXIT_SUCCESS);
        }

        // /* --processes */
        // if (opt_idx == 1)
        // {
        //   /* strtol - non-deprecated way to convert from string to integer */
        //   /*        - return 0 if no digits are provided to be converted   */
        //   processes = strtol(optarg, NULL, 10);
        // }

        break;
      }

      case 'h':
      {
        print_help();
        exit(EXIT_SUCCESS);
        break;
      }

      // case 'p':
      // {
      //   /* strtol - non-deprecated way to convert from string to integer */
      //   /*        - return 0 if no digits are provided to be converted   */
      //   processes = strtol(optarg, NULL, 10);
      //   break;
      // }

      case '?':
      {
        /* no need to do anything here */
        /* opt_long handles invalid options by default */
        exit(EXIT_FAILURE);
      }

      default:
        printf("something went wrong: %d\n", opt);
        exit(EXIT_FAILURE);
    }
  }
}

static void print_help()
{
  printf("NAME\n\tProiect PCD - User\n\n");
  printf("DESCRIPTION\n\tINET user client.\n\n");
  printf("SYNOPSIS\n\tuser [options]\n\n");

  printf("OPTIONS\n");
  printf("\t-h\t--help\t\tShows this page. If provided, all other options are ignored.\n\n");
  printf("EXAMPLE USAGE:\n\t./user\n\n");
}