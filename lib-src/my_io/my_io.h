#pragma once

#include <unistd.h>

size_t myRead(const char* file_path, char** buffer, size_t buffer_size);

size_t myWrite(const char* file_path, char* buffer, size_t buffer_size);