cmake_minimum_required(VERSION 3.22.1)

project( proiect-pcd )

add_compile_definitions(
  SERVER_IP="192.168.174.48"
  SERVER_PORT=42069
  MAX_LINE_SIZE=1024
)

# build server
add_subdirectory(server)

# build client
add_subdirectory(client)
