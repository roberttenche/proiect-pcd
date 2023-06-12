#pragma once

#include <stdlib.h>

#define PROCESSING_TYPE_BLUR 1
#define PROCESSING_TYPE_UPSCALE 2
#define PROCESSING_TYPE_DOWNSCALE 4

typedef struct header
{
  char file_extension[20];
  size_t file_size;
  unsigned char processing_type;
} header_t;

// client -> server [header]
// client <- server [ACK]
// UNTIL ALL PACKETS SENT
// client -> server [N*packet]
// client <- server [ACK N]
//  ELSE
//    client <- server [re-request P1,P2..PN]
// client -> server [EOT]
// client <- server [ACK]
