find_package( OpenCV REQUIRED )

add_executable(server server.cpp )

target_include_directories(server PRIVATE
  ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(server PRIVATE
  ${OpenCV_LIBS}
)