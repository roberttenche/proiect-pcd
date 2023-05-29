#pragma once

typedef enum user_type {

  NONE = 0, // used for  initialization only

  ADMIN = 1,
  INET_USER = 2,
  REST_USER = 3,

  USER_COUNT  // do not delete, do not change

} user_type_t;



typedef struct header
{

  user_type_t usr_type;
  int num_of_packets;

} header_t;



typedef struct packet
{

  char buffer[1024];

} packet_t;

// client -> server [header]
// client <- server [ACK]
// UNTIL ALL PACKETS SENT
// client -> server [N*packet]
// client <- server [ACK N]
//  ELSE
//    client <- server [re-request P1,P2..PN]
// client -> server [EOT]
// client <- server [ACK]
