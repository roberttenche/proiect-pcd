### USER  ###
add_executable(user
  user/user.c
  user/arg_parser/arg_parser.c
)

target_include_directories(user PRIVATE
  ${CMAKE_SOURCE_DIR}/lib-src/my_io
  user/arg_parser
)

target_link_directories(user PRIVATE
  ${CMAKE_SOURCE_DIR}/lib-src/my_io
)

target_link_libraries(user PRIVATE
  my_io
)

### ADMIN ###
add_executable(admin
  admin/admin.c
)
