#include "arg_parser.h"

#include <stdio.h>  /* for printf */
#include <string.h>
#include <stdlib.h> /* for exit */
#include <getopt.h> /* for getopt */

static struct option opt_long[] = {
  {"help",      no_argument,             0, 0},
  {"file",      required_argument,       0, 0},
  {"blur",      required_argument,       0, 0},
  {"upscale",   required_argument,       0, 0},
  {"downscale", required_argument,       0, 0}
};

static void print_help();

extern char** environ;

void arg_parse(int argc, char* argv[], char* envp[], char** filePath, int* apply_blur, int* apply_upscale, int* apply_downscale)
{
  int opt = 0;
  int opt_idx = 0;

  /* read all options and arguments form the command line*/
  while ((opt = getopt_long(argc, argv, "hf:b:u:d:", opt_long, &opt_idx)) != -1)
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

        /* --file */
        if (opt_idx == 1)
        {
          int len = strlen(optarg);
          *filePath = (char*)malloc(len);
          strncpy(*filePath, optarg, len);
        }

        /* --blur */
        if (opt_idx == 2)
        {
          /* strtol - non-deprecated way to convert from string to integer */
          /*        - return 0 if no digits are provided to be converted   */
          *apply_blur = strtol(optarg, NULL, 10);
        }

        /* --upscale */
        if (opt_idx == 3)
        {
          /* strtol - non-deprecated way to convert from string to integer */
          /*        - return 0 if no digits are provided to be converted   */
          *apply_upscale = strtol(optarg, NULL, 10);
        }

        /* --downscale */
        if (opt_idx == 4)
        {
          /* strtol - non-deprecated way to convert from string to integer */
          /*        - return 0 if no digits are provided to be converted   */
          *apply_downscale = strtol(optarg, NULL, 10);
        }

        break;
      }

      case 'h':
      {
        print_help();
        exit(EXIT_SUCCESS);
        break;
      }

      case 'f':
      {
        int len = strlen(optarg);
        *filePath = (char*)malloc(len);
        strncpy(*filePath, optarg, len);
        break;
      }

      case 'b':
      {
        /* strtol - non-deprecated way to convert from string to integer */
        /*        - return 0 if no digits are provided to be converted   */
        *apply_blur = strtol(optarg, NULL, 10);
        break;
      }

      case 'u':
      {
        /* strtol - non-deprecated way to convert from string to integer */
        /*        - return 0 if no digits are provided to be converted   */
        *apply_upscale = strtol(optarg, NULL, 10);
        break;
      }

      case 'd':
      {
        /* strtol - non-deprecated way to convert from string to integer */
        /*        - return 0 if no digits are provided to be converted   */
        *apply_downscale = strtol(optarg, NULL, 10);
        break;
      }

      case '?':
      {
        /* no need to do anything here */
        /* opt_long handles invalid options by default */
        exit(EXIT_FAILURE);
      }

      default:
      {
        printf("something went wrong: %d\n", opt);
        exit(EXIT_FAILURE);
      }

    }
  }

}

static void print_help()
{
  printf("NAME\n\tProiect PCD - User\n\n");
  
  printf("DESCRIPTION\n\tINET user client.\n\n");
  
  printf("SYNOPSIS\n\tuser [options]\n\n");

  printf("OPTIONS\n");
  printf("\t-h\t--help\t\tShows this page. If provided, all other options are ignored.\n");
  printf("\t-f\t--file\t\tSpecifies input file path.\n");
  printf("\t-b\t--blur\t\tApplies blur to specified file.\n");
  printf("\t-u\t--upscale\tApplies upscale to specified file.\n");
  printf("\t-d\t--downscale\tApplies downscale to specified file.\n\n");

  printf("EXAMPLE USAGE:\n\t./user --file video.mp4 --blur\n\n");
}