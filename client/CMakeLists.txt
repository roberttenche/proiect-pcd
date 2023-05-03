### USER  ###
add_executable(user
  user.c
)

target_include_directories(user PRIVATE
  ${CMAKE_SOURCE_DIR}/lib-src/my_io
)

target_link_directories(user PRIVATE
  ${CMAKE_SOURCE_DIR}/lib-src/my_io
)

target_link_libraries(user PRIVATE
  my_io
)

### ADMIN ###
add_executable(admin
  admin.c
)
