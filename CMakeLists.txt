cmake_minimum_required(VERSION 3.22.1)

project( proiect-pcd )

add_compile_definitions(
  SERVER_IP="127.0.0.1"
  SERVER_PORT=42069
)

# build libs
add_subdirectory(lib-src)

# build server
add_subdirectory(server)

# build client
add_subdirectory(client)
