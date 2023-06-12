find_package( OpenCV REQUIRED )

add_compile_definitions(
  MAX_TCP_CONNECTIONS=1024
)

add_executable(server
  main.cpp
  sig_handler/sig_handler.cpp
  tcp_handler/tcp_handler.cpp
  udp_handler/udp_handler.cpp
  processing/blur/blur.cpp
  processing/downscale/downscale.cpp
  processing/upscale/upscale.cpp
)

target_include_directories(server PRIVATE
  ${OpenCV_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/common_types
  sig_handler
  tcp_handler
  udp_handler
  processing/blur
  processing/downscale
  processing/upscale
)

target_link_libraries(server PRIVATE
  ${OpenCV_LIBS}
)